# CSL_WEC Task_ Swaraj Shinde
Custom security layer protocol, inspried by TLS


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

  

