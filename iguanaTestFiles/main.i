bit:32 main()
{
    bit:1 boolean = 0;
    bit:2 boolean2 = 0;
    
    bit:31 hey = ((50 | boolean) ^ 20);
    bit:7 hmm = 50 ^ 20;
    bit:6 lol = 63;
    bit:31 hey2 = ((50 | boolean) ^ 20);

    bit:63 lol5;
    lol5 = 5 + 6 + lol;


    print(hmm, hey, lol5);
}
