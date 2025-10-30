// idk the real name of this bitch
double mean_squared_error_nl(double output_activation, double expected_output) {
  double err = output_activation - expected_output;
  return err * err;
}
double mean_squared_error_nl_d(double output_activation, double expected_output) {
  return 2 * (output_activation - expected_output);
}

double mean_squared_error_w_constant_nl(double output_activation, double expected_output) {
  double err = output_activation - expected_output;
  return err * err * 0.2;
}

double abs_nl(double output_activation, double expected_output) {
  double err = output_activation - expected_output;
  if (err < 0) {
    err *=-1;
  }
  return err;
}

