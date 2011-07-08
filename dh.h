/******************************************
 * Diffie-Hellman Key Exchange
 *
 * This is used to transmit data or a network securely.
 * The server must be using D-H as well, or else transmission will not work.
 *
 * All dh_* functions (i.e.: dh_encrypt()) are public functions.
 *
A - Host/server
B - Client

P, G, Pa, Pb - public (sent via clear text)
Ka, Kb, Ra, Rb - Private (not sent via clear text)

[Ra & Rb = private keys, Ka & Kb = secret keys for the session]

- 3 numbers are generated by A: P, G, Pa
- B generates Pb
- P & G are sent to B
- A generates Ra, B generates Rb
- A sends Pa, B sends Pb
- A creates Ka, B creates Kb
- A encrypts data using Ka (Ea)
- B receives Ea, decrypts using Kb
- B encrypts data using Kb (Eb)
- A receives Eb, decrypts using Ka

 * Please see appropriate source files for network information.
 ******************************************/
#include "debug.h"
#include "bigint.h"
#include "random.h"
#include <stdint.h>

/**
 * Since uint64_t is not supported by gcc's pow(), self-made pow is done.
 **/
uint64_t pow64(uint64_t a, uint64_t b){
	uint64_t v = a;

	while(b > 0){
		v *= a;
		b--;
	}

	return v;
}

/**
 * pubkey()
 * secret:	The secret the user generated at beginning	[in]
 * g:		The primitive root of P (constant)		[in]
 * p:		The prime number (constant)			[in]
 *
 * Formula: PK = (g ^ secret)(mod p)
 *
 * Generates a public key based on the given information that is to be shared with other party.
 **/
uint64_t pubkey(uint64_t secret, uint64_t g, uint64_t p){
	uint64_t tmp = pow64(g, secret);
	tmp = tmp % p;

	return tmp;
}

/**
 * ke_secret()
 * b:		The primitive root of P (constant)		[in]
 * secret:	Secret received from other party (NOT own)	[in]
 * p:		Prime number (constant)				[in]
 *
 * Formula: Secret Key = (b ^ secret)(mod p)
 *
 * This is the secret key used to encrypt and decrypt data sent and received.
 **/
uint64_t ke_secret(uint64_t b, uint64_t secret, uint64_t p){
	uint64_t tmp = pow64(b, secret);
	tmp = tmp % p;

	return tmp;
}

/**
 * prime()
 * v:	The number to check against.	[in]
 *
 * The D-H KE specifies that p MUST be prime...0 if not, 1 if so.
 **/
int prime(uint64_t v){
	int t = 3;
	int n = 9;

	if((v == 2) || ((v & 1) == 0))
		return 0;

	while(n < v){
		if((v % t) == 0)
			return 0;

		t += 2;
		n = t * t;
	}

	if(n == v)
		return 0;

	return 1;
}

/**
 * gen_p()
 * No varaibles passed.
 *
 * The D-H KE requires a prime number be generated for modulo computations.
 *
 * Returns the prime number found.
 **/
uint64_t gen_p(void){
	tvstart = gettime();

	int bit = 8192;
	int size = bit / 2;

	gmp_randstate_t grand;

	mpz_t p;
	mpz_init(p);

	char *p_str = (char*)malloc(sizeof(char) * (size + 1));

	p_str[0] = '1';

	gmp_randinit_default(grand);
	gmp_randseed_ui(grand, bit * size * tvstart.tv_usec);

	// for() loop was here (for(; i < bits; i++){}), took it out as doesn't seem to be needed
//	mpz_urandomb(randres, grand, bit);
//	mpz_out_str(stdout, 10, randres);
//	printf("\n");

	birandom(bit, p);
//mpz_out_str(stdout, 10, p);
	p_str[4096] = '\0';

//	mpz_set_str(randres, p_str, 2);
//	mpz_set_str(p, p_str, 2);
//	mpz_nextprime(p, p);

	mpz_get_str(p_str, 10, p);

	gmp_randclear(grand);
D(("p = %s", p_str));
	mpz_clear(p);
	free(p_str);

	tvend = gettime();
	bottleneck(&tvstart, &tvend, "Generating P");
	return 0;
}

/**
 * gen_g()
 * p:	The prime number to get primitive root for	[in]
 *
 * G is based off of the prime number P, and is the primitive root.
 *
 * This is probably far from good practice on doing this.
 **/
uint64_t gen_g(uint64_t p){
	uint64_t tmp;

	return tmp;
}
