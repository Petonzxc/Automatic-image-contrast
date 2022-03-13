# Automatic Image Contrast Using OpenMP

## Information

Description of the problem: The image may have poor contrast: not the entire range of values ​​is used, but only part of it. For example, if the darkest points of the image have a value of 20, not 0.

The task is to change the pixel values ​​in such a way as to obtain maximum contrast: stretch the range of values ​​to [0; 255], but do not change the hue (that is, in color images, you need to change the R, G, and B channels in the same way).

algorithm_parameters = coefficient
  
When calculating the stretch, ignore a certain proportion (by number) of the darkest and lightest points (for RGB in each channel separately): <coefficient> (value range [0.0, 0.5)). This allows you to ignore noise that is invisible to the eye, but interferes with automatic contrast adjustment. Range stretching should be performed with saturation so that the ignored pixels do not go beyond [0; 255].
  
If the image consists of one color, then it is not processed (does not change).
  
You can also read the report on this laboratory work in Russian (hw5.docx). 
  
## Image storage format
Input and output image format: PNM (P5 or P6).
  
## Input format
Arguments are passed to the program via the command line:
hw5.exe <number_of_threads> <input_file_name> <output_file_name> [<algorithm_parameters>].
  
The number of threads can be 0 or more. 0 is the default number of threads.
  
## Output Format
At the output, you get the image processed by the algorithm, and the measurement results are displayed on the console. 
  
Output format: "Time (%i thread(s)): %g ms\n"
