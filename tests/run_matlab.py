import subprocess
r = subprocess.run([r"C:\Program Files\MATLAB\R2025b\bin\matlab.exe", "-batch", "run('tests/mat_file_generator.m')"], shell=True)
