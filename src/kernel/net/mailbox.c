/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <arch/nic_driver_if.h>
#include <dev/net/net.h>
#include <net/mailbox.h>
#include <lwip/tcp.h>
#include <lwip/timeouts.h>
#include <nanvix/kernel/config.h>

#if __NANVIX_HAS_NETWORK

/**
 * @brief Output mailbox structure
 */
struct output_mailbox
{
    struct tcp_pcb* pcb;
    bool initialized;
    bool connected;
};

/**
 * @brief Input mailbox structure
 */
PRIVATE struct
{
    struct tcp_pcb* pcb;
    bool initialized;

    uint8_t recv_buffer[MAILBOX_RECV_BUFFER_NB][MAILBOX_MSG_SIZE];
    uint32_t read_index;
    uint32_t write_index;
} input_mailbox = {
    .pcb = NULL,
    .initialized = false,
    .read_index = 0,
    .write_index = 0,
};

/**
 * @brief Output mailbox list.
 */
PRIVATE struct output_mailbox output_mailboxes[OUTPUT_MAILBOX_NB];

/**
 * @brief Processes a received packet.
 */
PRIVATE err_t input_mailbox_recv(void *arg, struct tcp_pcb *tpcb,
    struct pbuf *p, err_t err)
{
    int nummsg;      /* Number of messages. */
    uint32_t wrtidx; /* Write index.        */
    int cpyidx;      /* Copy index.         */
    struct pbuf *q;  /* Packet buffer.      */

	UNUSED(arg);
	UNUSED(err);

    /* Packet length should be multiple of MSG_SIZE. */
    if (p->tot_len % MAILBOX_MSG_SIZE)
        return (-EINVAL);

    nummsg = p->tot_len / MAILBOX_MSG_SIZE;
    cpyidx = 0;

    /* Check that there is enough space into the buffer */
    wrtidx = input_mailbox.write_index;
    for (int i = 0; i < nummsg; i++)
    {
        wrtidx = (wrtidx + 1) % MAILBOX_RECV_BUFFER_NB;
        if (wrtidx == input_mailbox.read_index)
        {
            kprintf("Mailbox receive buffer full");
            return (ERR_INPROGRESS);
        }
    }

    /* Fill the buffers with the received packets */
    for (q = p; q != NULL; q = q->next)
    {
        /* Fill buffer */
        for (int i = 0; i < q->len; i++)
        {
            if (cpyidx >= MAILBOX_MSG_SIZE)
            {
                input_mailbox.write_index++;
                cpyidx = 0;
            }

            input_mailbox.recv_buffer[input_mailbox.write_index][cpyidx++] =
                ((u8_t *)q->payload)[i];
        }
    }

    input_mailbox.write_index = (input_mailbox.write_index + 1) %
        MAILBOX_RECV_BUFFER_NB;

    /* Ack that we processed the packets */
    tcp_recved(tpcb, p->tot_len);
    if (p != NULL)
        pbuf_free(p);

    return (ERR_OK);
}

/**
 * @brief Accepts a TCP connection.
 */
PRIVATE err_t input_mailbox_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	UNUSED(arg);
	UNUSED(err);

    /* Invalid PCB. */
    if (newpcb != NULL)
    {
        tcp_setprio(newpcb, TCP_PRIO_NORMAL);
        tcp_recv(newpcb, input_mailbox_recv);
    }

	return (ERR_OK);
}

/**
 * @brief Finds a free output mailbox and returns.
 *
 * @return Returns the first output mailbox available, if none,
 * returns a negative number.
 */
PRIVATE int find_free_output_mailbox(void)
{
    for (int i = 0; i < OUTPUT_MAILBOX_NB; i++)
    {
        if (!output_mailboxes[i].initialized)
            return (i);
    }

    return (-EAGAIN);
}

/**
 * @brief Assigns an output mailbox to a connected state.
 */
PRIVATE err_t output_mailbox_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct output_mailbox* mb; /* Output mailbox. */

    UNUSED(arg);
    UNUSED(tpcb);
    UNUSED(err);

    mb = (struct output_mailbox*) arg;
    mb->connected = true;

    return (ERR_OK);
}

/**
 * @brief Converts the node number to IP address.
 *
 * @param node Node number to be converted.
 *
 * @return Returns the equivalent IP address.
 */
PRIVATE char* node_to_ip(int node)
{
    int i;                     /* Loop index.       */
    int divisor;               /* Current divisor.  */
    static char ip[16] = {0};  /* IP Address.       */

    /* Fill the first octects. */
    for (i = 0; NETWORK_REMOTE_ADDR[i] != '\0'; i++)
        ip[i] = NETWORK_REMOTE_ADDR[i];

    /* Calculate the space required. */
    divisor = 0;
    if (node / 100)
        divisor = 100;
    else if (node / 10)
        divisor = 10;
    else
        divisor = 1;

    /* Fill the last octect. */
    while (divisor)
    {
        ip[i++] = (node / divisor) + '0';
        node %= divisor;
        divisor /= 10;
    }

    ip[i] = '\0';
    return (ip);
}

/**
 * Initialize a mailbox into the mailboxes array
 *
 * @return Returns 1 upon successfull mailbox creation, a negative
 * number if an error occured
 */
PUBLIC int net_mailbox_create(int local)
{
    err_t error; /* Error code. */

    UNUSED(local);

    /* Mailbox already initialized first. */
    if (input_mailbox.initialized)
    {
        kprintf("The input mailbox already have been initialized");
        return (-EBUSY);
    }

    input_mailbox.pcb = tcp_new();
    if ((error = tcp_bind(input_mailbox.pcb, IP4_ADDR_ANY, MAILBOX_PORT))
        != ERR_OK)
    {
        kprintf("Error when binding the tcp pcb. Error: %d", (int) error);
        return (-EAGAIN);
    }

	input_mailbox.pcb = tcp_listen(input_mailbox.pcb);
	tcp_accept(input_mailbox.pcb, input_mailbox_accept);
    input_mailbox.read_index = 0;
    input_mailbox.write_index = 0;

    input_mailbox.initialized = true;

    return (1);
}

/**
 * Initialize a mailbox connected the given remote
 *
 * @return Returns the mailbox ID of the created mailox, a negative
 * number if an error occured
 */
PUBLIC int net_mailbox_open(int remote)
{
    struct output_mailbox* mb; /* Output mailbox.        */
    err_t error;               /* Error code.            */
    int idx;                   /* Ooutput mailbox index. */
    ip4_addr_t ipaddr;         /* IP Address.            */

    /* Gets the first output available. */
    idx = find_free_output_mailbox();
    if (idx < 0)
    {
        kprintf("Error: no empty mailbox available");
        return (-EAGAIN);
    }

    mb = &output_mailboxes[idx];
    mb->pcb = tcp_new();
    tcp_arg(mb->pcb, mb);

    ip4addr_aton(node_to_ip(remote + 1), &ipaddr);

    /* Establishes the connection. */
    if ((error = tcp_connect(mb->pcb, &ipaddr, MAILBOX_PORT,
        output_mailbox_connected)) != ERR_OK)
    {
        kprintf("Error when connecting the mailbox : %d", (int)error);
        tcp_close(mb->pcb);
        return (-EAGAIN);
    }

    mb->initialized = true;
    return (idx);
}

/**
 * Write the message contained in buffer of size size to the
 * mailbox mbxid
 *
 * @return Upon success, returns the size write, otherwise, a negative
 * number.
 */
PUBLIC int net_mailbox_awrite(int mbxid, const void * buffer, size_t size)
{
    struct output_mailbox mb;             /* Output mailbox.       */
    u8_t padded_buffer[MAILBOX_MSG_SIZE]; /* Padded output buffer. */
    const void* msg;                      /* Buffer pointer.       */
    err_t error;                          /* Error code.           */

    mb = output_mailboxes[mbxid];

    if(network_is_new_packet())
        nic_driver_if_input(&net_interface);
    sys_check_timeouts();

    /* Mailbox not initialized. */
    if (!mb.initialized)
    {
        kprintf("Can't write, the mailbox %d is not initialized", mbxid);
        return (-EBADF);
    }

    /* Mailbox not connected. */
    if (!mb.connected)
        return (-EBADF);

    /* Padding */
    if (size != MAILBOX_MSG_SIZE)
    {
        kmemcpy(padded_buffer, buffer, size);
        msg = padded_buffer;
    }
    else
        msg = buffer;

    /* Writing the message */
    if ((error = tcp_write(output_mailboxes[mbxid].pcb, msg, size,
        TCP_WRITE_FLAG_COPY)) != ERR_OK)
    {
        return (-EAGAIN);
    }

    if ((error = tcp_output(output_mailboxes[mbxid].pcb)) != ERR_OK)
        return (-EAGAIN);

    return (size);
}

/**
 * Read up to size bytes from the input_mailbox into buffer. If you
 * read less than the total length of the message, the rest of the
 * message is discarded
 *
 * @return Upon success, returns the size read, otherwise, a negative
 * number.
 */
PUBLIC int net_mailbox_aread(int mbxid, void * buffer, size_t size)
{
    UNUSED(mbxid);

    if(network_is_new_packet())
        nic_driver_if_input(&net_interface);
    sys_check_timeouts();

    /* Mailbox uninitialized. */
    if (!input_mailbox.initialized)
    {
        kprintf("Input mailbox uninitialized");
        return (-EAGAIN);
    }

    tcp_accept(input_mailbox.pcb, input_mailbox_accept);

    /* Invalid indexes. */
    if (input_mailbox.read_index == input_mailbox.write_index)
        return (-EAGAIN);

    kmemcpy(buffer, input_mailbox.recv_buffer[input_mailbox.read_index], size);

    input_mailbox.read_index = (input_mailbox.read_index + 1)
        % MAILBOX_RECV_BUFFER_NB;

    return (size);
}

/**
 * Remove the mbxid mailbox to the mailboxes array if it was
 * created with mailbox_create
 *
 * @return Returns 1 for a successfull unlink, a negative number
 * if an error occured.
 */
PUBLIC int net_mailbox_unlink(int mbxid)
{
    err_t error; /* Error code. */

    UNUSED(mbxid);

    /* Mailbox uninitialized. */
    if (!input_mailbox.initialized)
    {
        kprintf("The input mailbox is already closed");
        return (-EBADF);
    }

    /* Close connection. */
    if ((error = tcp_close(input_mailbox.pcb)) != ERR_OK)
    {
        kprintf("Error when closing the input mailbox : %d", (int)error);
        return (-EAGAIN);
    }

    input_mailbox.initialized = false;

    return (1);
}

/**
 * Remove the mbxid mailbox to the mailboxes array if it was
 * created with mailbox_open.
 *
 * @return Returns 1 for a successfull unlink, a negative number
 * if an error occured.
 */
PUBLIC int net_mailbox_close(int mbxid)
{
    struct output_mailbox* mb; /* Mailbox output. */

    mb = &output_mailboxes[mbxid];

    /* Mailbox uninitialized. */
    if (!mb->initialized)
    {
        kprintf("The %d mailbox is already closed", mbxid);
        return (-1);
    }

    tcp_close(mb->pcb);
    mb->connected = false;
    mb->initialized = false;
    return (1);
}

#endif
