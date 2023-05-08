kernel void multiply_matrices(
	global const double *a,
	global const double *b,
	global double *result,
	const int result_width, const int height_b,
	const int width_a, const int width_b
) {
	const int i = get_global_id(0);
	const int j = get_global_id(1);
	int k;
	double tmp = 0;
	for (k = 0; k < height_b; ++k) {
		tmp += a[i * width_a + k] * b[k * width_b + j];
	}
	result[i * result_width + j] = tmp;
}