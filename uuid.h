#ifndef UUID_H
#define UUID_H
typedef unsigned char uuid_t[16];

void uuid_copy(uuid_t dst, const uuid_t src){
  unsigned char	*cp1;
  const unsigned char	*cp2;
  int	i;
  for (i=0, cp1 = dst, cp2 = src; i < 16; i++)
    *cp1++ = *cp2++;
}

#endif
