# Automatic Image Contrast Using OpenMP

## Information

Description of the problem: The image may have poor contrast: not the entire range of values ​​is used, but only part of it. For example, if the darkest points of the image have a value of 20, not 0.

The task is to change the value of the observed in such a way as to obtain a contrast representation: stretch the range of values ​​to [0; 255], but it does not change the hue (i.e., in color images, you need to change R, G, and B in the same way).
<algorithm_parameters> = <coefficient>
  
When calculating the measurement of the proportion (by number) of the darkest and brightest points (for RGB in each channel separately): <coefficient> (measurement range [0.0, 0.5)). This allows you to ignore noise that is invisible to the eye, but eliminates the change in contrast. Stretching is performed with saturation so that the ignored pixels go beyond [0; 255].
  
If the image is contained in one color, then it does not cause a reaction (does not change).
  
## Image storage format
Input and output image format: PNM (P5 or P6).
  
## Input format
Arguments are passed to the program via the command line:
hw5.exe <number_of_threads> <input_file_name> <output_file_name> [<algorithm_parameters>]
The number of threads can be 0 or more. 0 is the default number of threads.
