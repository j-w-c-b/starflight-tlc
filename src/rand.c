static int random_seed;

/* Override rand() with an LCG using the MSVC parameters
 * https://en.wikipedia.org/wiki/Linear_congruential_generator
 * so that linux/mac have the same behavior as the original Windows version
 */
extern
int
rand()
{
     random_seed = random_seed * 214013 + 2531011;
     return (random_seed >> 16) & 0x7fff;
}

extern
void
srand(unsigned int seed)
{
    random_seed = seed;
}

