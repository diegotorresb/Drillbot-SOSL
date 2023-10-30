%MATLAB Code for Serial Communication with Arduinofclose(instrfind);
delete(instrfind);
keep = true;
x=serialport('COM8', 9600);%%remember, the com port must match. use clear all to return access to arduino/matlab
fopen(x);
pause(1);
while (keep == true)
    a = input('Enter "e" to exit, Enter x or y:', "s");
    if (a == 'e')
        break;
    end
    fprintf(x, '%c', a);
    b = input("Enter the number of revs (100 = 90 deg)? Ex: 100 -> <100>, enter 'exit' to exit ", "s");
    if (b == "exit")
        break;
    end
    fwrite(x, b);
    pause(0.1);
end
fclose(x);
