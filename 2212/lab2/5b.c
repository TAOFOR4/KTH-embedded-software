#include <stdio.h>
#include "circular_buffer.h" /* defines data type token as uint8_t */

/* Definition of the channel */
typedef cbuf_handle_t channel;

/* Definition of function 'createFIFO' */
channel createFIFO(token* buffer, size_t size){
return circular_buf_init(buffer, size);
}

/* Definition of the functions readToken and writeToken */
int readToken(channel ch, token* data) {
return circular_buf_get(ch, data);
}

void writeToken(channel ch, token data) {
circular_buf_put(ch, data);
}


void actor11SDF(int consum, int prod,
channel* ch_in, channel* ch_out,
void (*f) (token*, token*))
{
token input[consum], output[prod];
int i;
for(i = 0; i < consum; i++) {
readToken(*ch_in, &input[i]);
}
f(input, output);
for(i = 0; i< prod; i++) {
writeToken(*ch_out, output[i]);
}
}

void actor12SDF(int consum, int prod1, int prod2,
channel* ch_in, channel* ch_out1, channel* ch_out2,
void (*f) (token*, token*, token*))
{
token input[consum], output1[prod1], output2[prod2];
int i;
for(i = 0; i < consum; i++) {
readToken(*ch_in, &input[i]);
}
f(input, output1, output2);
for(i = 0; i< prod1; i++) {
writeToken(*ch_out1, output1[i]);
}
for(i = 0; i< prod2; i++) {
writeToken(*ch_out2, output2[i]);
}
}

void actor21SDF(int consum1, int consum2, int prod,
channel* ch_in1, channel* ch_in2, channel* ch_out,
void (*f) (token*, token*, token*))
{
token input1[consum1], input2[consum2], output[prod];
int i;

for(i = 0; i < consum1; i++) {
readToken(*ch_in1, &input1[i]);
}
for(i = 0; i < consum2; i++) {
readToken(*ch_in2, &input2[i]);
}
f(input1, input2, output);
for(i = 0; i< prod; i++) {
writeToken(*ch_out, output[i]);
}
}

void actor22SDF(int consum1, int consum2, int prod1, int prod2,
channel* ch_in1, channel* ch_in2, channel* ch_out1, channel* ch_out2,
void (*f) (token*, token*, token*, token*))
{
 token input1[consum1], input2[consum2], output1[prod1], output2[prod2];
 int i;
 for(i = 0; i < consum1; i++) {
 readToken(*ch_in1, &input1[i]);
 }
 for(i = 0; i < consum2; i++) {
 readToken(*ch_in2, &input2[i]);
 }
 f(input1, input2, output1, output2);
 for(i = 0; i< prod1; i++) {
 writeToken(*ch_out1, output1[i]);
 }
 for(i = 0; i< prod2; i++) {
 writeToken(*ch_out2, output2[i]);
 }
 }

// Original ForSyDe model:
// 

/*
p_4 = actor12SDF 1 (3 ,1) f_4
where f_4 [ x ] = ([ x , x +1 , x +2] ,[ x ])
↓↓↓↓↓
void f_4(token* in, token* out1, token* out2) {
out1[0] = in[0];
out1[1] = in[0] + 1;
out1[2] = in[0] + 2;
out2[0] = in[0];
}
*/

/*
actor_a = actor11SDF 2 1 f_1 where
f_1 [x, y] = [x + y]*/
void f_1(token* in, token* out) {
out[0] = in[0]+in[1];
}

/*actor_b = actor11SDF 1 2 f_2 where
f_2 [x] = [x, x+1]*/
void f_2(token* in, token* out) {
out[0] = in[0];
out[1] = in[0] + 1;
}

/*actor_c = actor21SDF (2,1) 1 f_3 where
f_3 [x, y] [z] = [x + y + z]*/
void f_3(token* in1, token* in2, token* out) {
out[0] = in1[0]+in1[1]+in2[0];
}

/* = actor22SDF (2,1) (1,2) f_4 where
f_4 [x, y] [z] = ([x + y + z], [x + y, x + y + z])*/
void f_4(token* in1, token* in2, token* out1, token* out2) {
out1[0] = in1[0]+in1[1]+in2[0];
out2[0] = in1[0]+in1[1];
out2[1] = in1[0]+in1[1]+in2[0];
}


/*MAIN FUNCTION*/
int main() {
  token input1,input2;
  token output;
  int i, j, k;

/* Create FIFO-Buffers for signals */
token* buffer_s_in1 = malloc(2 * sizeof(token));
channel s_in1 = createFIFO(buffer_s_in1, 2);

token* buffer_s_in2 = malloc(1 * sizeof(token));
channel s_in2 = createFIFO(buffer_s_in2, 1);

token* buffer_s_out = malloc(2 * sizeof(token));
channel s_out = createFIFO(buffer_s_out, 2);

token* buffer_s_1 = malloc(2 * sizeof(token));
channel s_1 = createFIFO(buffer_s_1, 2);

token* buffer_s_2 = malloc(2 * sizeof(token));
channel s_2 = createFIFO(buffer_s_2, 2);

token* buffer_s_3 = malloc(1 * sizeof(token));
channel s_3 = createFIFO(buffer_s_3, 1);

token* buffer_s_4 = malloc(1 * sizeof(token));
channel s_4 = createFIFO(buffer_s_4, 1);


/* Put initial tokens in channel s_4 */
writeToken(s_4, 0);
/*actor should follow schedule:aabdc*/
while(1) 
{
/* Read 5 input tokens ???*/
for(i = 0; i < 2; i++) /*aa*/
{
 printf("Read 2 input tokens for input1:\n");
 for(j = 0; j < 2; j++) 
  {
  scanf("%d", &input1);
  writeToken(s_in1, input1);
  }
/* actor_a */
  actor11SDF(2, 1, &s_in1, &s_1, f_1);
  //actor21SDF(2, 1, &s_in1, &s_1, f_1); //-Akash
}
printf("Read 1 input tokens for input2:\n");
scanf("%d", &input2);
writeToken(s_in2, input2);

/* actor_b */
actor11SDF(1, 2, &s_in2, &s_2, f_2);


/* actor_c */
actor21SDF(2, 1, 1, &s_1, &s_4, &s_3, f_3);

/* actor_d */
actor22SDF(2, 1, 1, 2, &s_2, &s_3, &s_4, &s_out, f_4);
/* Write output tokens */
printf("Output: \n");

/*
for(k = 0; k< 2; k++) {
readToken(s_out, &output);
printf("%d\n", output);}
*/
readToken(s_out, &output);
printf("%d\n ", output);
readToken(s_out, &output);  //Akash commented
printf("%d\n ", output); //Akash commented
}

//return 0;
}


