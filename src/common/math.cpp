unsigned int vabs(int a)
{
	unsigned int value;

	if (a >= 0)
		value = a;
	else
		value = -a;

	return value;
}