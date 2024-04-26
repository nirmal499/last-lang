
fun fib(n)
{
    var previous_previous_number = 0;
    var previous_number = 0;
    var current_number = 1;

    print previous_number;
    print current_number;

    var c = 2;
    while(c <= n)
    {
        previous_previous_number = previous_number;
        previous_number = current_number;
        current_number = previous_previous_number + previous_number;

        print current_number;

        c = c + 1;
    }

    return current_number;
}

var c = 5;
var b = "WE GOT";

print b;

print fib(c);