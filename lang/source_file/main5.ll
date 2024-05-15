fun count(number)
{
    var x = number;

    if(x == 0)
    {
        print "Done";
    }
    else
    {
        count(x - 1);
        print x;
        x = 32;
    }
}

count(3);