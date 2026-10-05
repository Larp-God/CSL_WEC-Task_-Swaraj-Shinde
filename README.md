# CSL_WEC Task_ Swaraj Shinde
Custom security layer protocol, inspried by TLS

# **To the evaluator, I have just implemented till Level 4, i could not complete the further Levels, however i did do the Bonus part of Level 3, i implemented HKDF in my code**

*Day 1* : 

Hello reader, so today i day 1 of starting with the assignment. I started with asking ChatGPT about the assignment adn to epxlain it to me and then after reading i boiled it down to the fact that i need to learn and use Socket Programming. Initially i tried learning it from ChatGPT but then i realsied it grazing over a lot of stuff and also that i am more of a visual elarner so i decided to search a video on youtube. I found the video of the channel "Nicholas Day" and i am deciding to follow this video series.

        What i learnt today : 
            (a) I learnt that even after closing the connection (after 4 way TCP Hanshake), the client waits for roughyl around 60 secs till freeing up the port. 
            Basically it locks down this port for a minute and it in this period if we run the server program again it shows the error of "address already in use". 
            Linux does this so taht the conenction is clsoed smoothly and that any "ghost" packets which ahve not been yet recieved by the client side dont get recivede by the next program.

            (b) Learnt the flow of the code and why we write what we write (i hope i didnt waste time on learning too much logic :| ).
            Flow : (1) create socket 
                   (2) setsockopt (so that you can reuse port immediately after starting, no port locking)
                   (3) intialise sockaddr_in and memset (padding)
                   (4) sin_family and sin_addr (using inet_pton : converting from string to binary big endian format)
                   (5) sin_port (using htons())
                   (6) bind()
                   (7) cleanup

            (c) I finally completed mini-task of setting up a very basic client-server connection.
                Made several errors along the way like kept sizeof() for a string insterad of using strlen().
                Kept forgetting the input parameters and so messed up there.
                One of the, i would say best, error i made was when i used accept() function.
                    In that for the address length parameter, i directly passed the legnth sizeof()
                         but i realized i have to pass a pointer.





*Day 2* : 

Today was a leap damn. I learnt the basic TCP Framing and understood how like stuff works. I had to take hte help of ChatGPT to learn and Gemini for troubleshooting because i couldn't find any concise video taht would teach me the required topics. This part was more verbose than yesterday but what i can see in general is that while writing code for networking we write many statements for like fail cases and printing the error so that's an interesting thing i learnt about this style of programming.

    What i learnt today: 
        (a) Learnt how i can create custom framing for TCP.
            One error i was doing continuously was that i was giving the datatype 'int' to variables,
            instead of uint8_t/uint32_t. I had understood how different they are but it just seems muscle memory.

        (b) Biggest lessong for today is that i shouldn't read Berserk and do tasks cuz mangas are too addictive :').


*Day 3* :
    Again a very verbose day, learnt the entirety of Diffie-Hellman key exchange starting from the theory from computerphile and then using ChatGPT to understand what we ar egonna implement in the assignment. I tried finding videos of where coding for DH key exchange is done but i could only find the theory videos and not the coding ones. This part was very exhaustive because there were a lot of steps and minute minute conversions.the last part i ended up copypasting from chatgpt because i had gotten tired of typing it by myself after reading the theory and i lowkey dont have much time left too.

*Day 4*: 
I implemented level 3 and level 4 today, this is as far as i go before deadline. I got very exhausted today. I learnt about how we cant directly use DH shared key as transmitting that directly over the network is a vulenrability. I leanrt how we split that in two 32 bit components and use them as encryption and authorisation key because DH has no protection against MiTM attacks. 
Level 4 was all about how we prevent a tampered message being accepted by our server. It reminded me of blockchain where we ahve a similar mechanism of calculating block hash so that the transaction cannot be manipulate3d when the block is formed from the mempool. It was fun to see how things start to repeat on a bigger scales. I ahd heavy reliance on ChatGPT today and i didn't type our the code by myself for the alst part because of exhaustion. I have udnerstood the code but i didn't type it out by myself i just studied the why and how.

Overall it was a fun journey and i learnt a lot more about networking than what i could've via just studying theory.

  

