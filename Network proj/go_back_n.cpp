#define MAX_SEQ 7
#include "go_back_n.h"
static boolean between(seq_nr a, seq_nr b, seq_nr c)
{
	if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
		return(true_a);
	else
		return(false_a);
}

static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
	frame s;

	s.info = buffer[frame_nr];
	s.seq = frame_nr;
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
	to_physical_layer(&s);
	start_timer(frame_nr);

}

void protocl(void) {
	seq_nr next_frame_to_send;
	seq_nr ack_expected;
	seq_nr frame_expected;
	frame r;
	packet buffer[MAX_SEQ + 1];
	seq_nr nbuffered;
	seq_nr i;
	event_type event;
	enable_network_layer();
	ack_expected = 0;
	next_frame_to_send =0;
	frame_expected = 0;
	nbuffered = 0;
	while (true) {
		wait_for_event(&event);
		switch (event) {
		case network_layer_ready:
			from_network_layer(&buffer[next_frame_to_send]);
			nbuffered = nbuffered + 1;
			send_data(next_frame_to_send, frame_expected, buffer);
			inc(next_frame_to_send);
			break;
		case frame_arrival:
			from_physical_layer(&r);
			if (r.seq == frame_expected) {
				to_network_layer(&r.info);
				inc(frame_expected);
				while (between(ack_expected, r.ack, next_frame_to_send)) {
					nbuffered = nbuffered - 1;
					stop_timer(ack_expected);
					inc(ack_expected);
				}
				break;
		case cksum_err:
			break;
		case timeout:
			next_frame_to_send = ack_expected;
			for (int i = 0; i <= nbuffered; i++)
			{
				send_data(next_frame_to_send, frame_expected, buffer);
				inc(next_frame_to_send);
			}
		}
		if (nbuffered<MAX_SEQ)
		{
			enable_network_layer();
		}
		else
		{
			disable_network_layer();
		}
	}
}