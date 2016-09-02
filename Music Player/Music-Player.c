#include <msp430.h>


// Port 1 Bits used for project
//----------------------------------
#define SPEAKER             BIT1	//speaker bit
#define playButton          BIT2	//play/pause the song
#define restartButton       BIT3	//restarts the song
#define slowdownButton      BIT4	//slows down the tempo of the song
#define speedupButton       BIT5	//speeds up the tempo of the song
#define switchsongButton    BIT7	//toggle between the songs
#define jLength             72      //length of joy to the world song
#define GoTLength           99      //length of game of thrones theme song
#define noteMask            0x1F

unsigned const char joySongNotesTime[jLength] =  {4, 3, 1, 6, 2, 4, 4, 6, 2, 6, 1, 2, 6, 1, 2, 6, 1, 2, 1, 2, 2, 2, 2, 1, 3, 
                                                      1, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 1, 6, 1, 
                                                      1, 2, 1, 2, 1, 2, 1, 1, 1, 6, 1, 1, 2, 4, 2, 3, 1, 2, 2, 4, 4, 6};

unsigned const char GoTSongNoteTime[GoTLength] = {24, 24, 4, 4, 16, 16, 4, 4, 96, 24, 24, 4, 4, 16, 24, 4, 4, 88, 1, 24, 24, 4, 4, 16, 
                                                  16, 4, 4, 96, 24, 24, 4, 4, 16, 24, 4, 4, 88, 24, 24, 4, 4, 16, 16, 4, 4, 8, 8, 4, 4, 
                                                  8, 8, 4, 4, 8, 8, 4, 4, 8, 8, 4, 4, 16, 16, 12, 12, 12, 12, 8, 8, 4, 4, 8, 8, 4, 4, 8, 
                                                  8, 4, 4, 8, 8, 4, 4, 48, 48, 48, 48, 48, 24, 24, 96, 48, 48, 48, 48, 48, 24, 24, 96};

unsigned const int joyToTheWorldNotes[jLength] = {956, 1013, 1137, 1276, 1432, 1517, 1703, 1912, 1276, 1137, 0,
												  1137, 1013, 0, 1013, 956, 0, 956, 0, 956, 1013, 1137, 1276, 0, 1276, 1432,
												  1517, 956, 0, 956, 1013, 1137, 1276, 0, 1276, 1432, 1517, 0, 1517, 0, 1517, 0,
												  1517, 0, 1517, 0, 1517, 1432, 1276, 1432, 1517, 1703, 0, 1703, 0, 1703, 0, 1703,
												  1517, 1432, 1517, 1703, 1912, 956, 1137, 1276, 1432, 1517, 1432, 1517, 1703, 1192};

unsigned const int GoTSongNotes[GoTLength] = {1276, 1911, 1607, 1432, 1276, 1911, 1607, 1432, 1703, 1432, 2145, 1607, 1703, 1432, 
                                              2145, 1607, 1703, 1911, 804, 638, 956, 804, 716, 638, 956, 804, 716, 851, 716, 1073, 
                                              804, 851, 716, 1073, 804, 851, 956, 638, 956, 804, 716, 638, 956, 804, 716, 851, 1276, 
                                              1073, 956, 851, 1276, 1073, 956, 851, 1276, 1073, 956, 851, 1276, 1073, 956, 716, 1073, 
                                              851, 804, 851, 1073, 956, 1276, 1204, 1073, 956, 1276, 1204, 1073, 956, 1276, 1204, 1073, 
                                              956, 1276, 1204, 1073, 478, 536, 956, 638, 1204, 804, 716, 638, 478, 536, 956, 638, 1204, 
                                              804, 851, 956};



unsigned char status = 2; 	//checks the status = 2 - song is playing
unsigned int currentSongLength = jLength;
const unsigned int *currentSong = joyToTheWorldNotes; //sets the current song to joy to the world
unsigned int currentSongID = 0;  //sets the current song ID (for the joy to the world = 0, GoT = 1)  
unsigned int noteCounter = 0;	//keeps track of the notes
unsigned int songCounter = 0;	//keeps track of the song lengths
float speed = 1.00;				

//restart function 
void restart(void);

//toggles between playing and pausing the song
void playAndPause(void) {
	//restart status is 3
    if (status == 3) {
    	restart();
    }

    //sets Timer A to Output mode 4 where it can toggle the outputs
	TACCTL0 ^= OUTMOD_4;
	status = !status;
}

//slows down the tempo of the song
void slowDown(void) {
    if (speed < 10.0) {
        speed += 0.25;
    }
}

//speed up the tempo of the song
void speedUp(void) {
    if(speed > 0.125) {
        speed -= 0.25;
    }
}

// restart current song from the beginning
void restart(void) {
    TA0CTL |= TACLR; //reset TimerA
    status = 2;
    TACCTL0 &= ~OUTMOD_4;
    noteCounter = 0;
    songCounter = 0;
    TA0CCR0 = joyToTheWorldNotes[(currentSong[0] & noteMask)]-1; // TAR=0 to TACCRO-1
}


//switches between the 2 songs
void switchSong(void) {
	//if playing the current song(joy to the world), switch to the other song
	if (currentSongID == 0){
		status = 2; //plays song
		currentSong = GoTSongNotes;
		currentSongLength = GoTLength;
		currentSongID = 1;
		restart();
	}
	else{
		status = 2;
		currentSong = joyToTheWorldNotes;
		currentSongLength = jLength;
		currentSongID = 0;
	    restart();
	}
}



void main() {
	WDTCTL =  (WDTPW +
					 WDTTMSEL +
					 WDTCNTCL +
					 0 +
					 1);

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;

//intitialize the timer A and the buttons
    TA0CTL |= TACLR;
    TA0CTL = (TASSEL_2 +
              ID_0 +
              MC_1);

	TA0CCTL0=0;


	IE1 |= WDTIE;

    P1SEL |= SPEAKER;
    P1DIR |= SPEAKER;

    P1OUT |= restartButton;
    P1REN |= restartButton;
    P1IES |= restartButton;
    P1IFG &= ~restartButton;
    P1IE  |= restartButton;

    P1OUT |= playButton;
    P1REN |= playButton;
    P1IES |= playButton;
    P1IFG &= ~playButton;
    P1IE  |= playButton;

    P1OUT |= slowdownButton;
    P1REN |= slowdownButton;
    P1IES |= slowdownButton;
    P1IFG &= ~slowdownButton;
    P1IE  |= slowdownButton;

    P1OUT |= speedupButton;
    P1REN |= speedupButton;
    P1IES |= speedupButton;
    P1IFG &= ~speedupButton;
    P1IE  |= speedupButton;

    P1OUT |= switchsongButton;
    P1REN |= switchsongButton;
    P1IES |= switchsongButton;
    P1IFG &= ~switchsongButton;
    P1IE  |= switchsongButton;


    restart();

    _bis_SR_register(GIE+LPM0_bits);
}

//interrupt that performs the functions of the 5 buttons on the breadboard
interrupt void buttonInterrupt(void) {
    if (P1IFG & restartButton) {
        P1IFG &= ~restartButton;
        restart();
    }
    else if (P1IFG & playButton) {
        P1IFG &= ~playButton;
        playAndPause();
    }
    else if (P1IFG & switchsongButton) {
        P1IFG &= ~switchsongButton;
        switchSong();
    }
    else if (P1IFG & speedupButton) {
		P1IFG &= ~speedupButton;
		speedUp();
    }
    else if (P1IFG & slowdownButton) {
		P1IFG &= ~slowdownButton;
		slowDown();
    }
}

//clocked at 8MHz on timer a all notes are based off 1/16th note
interrupt void WDT_interval_handler(void) {
	 //initial status
     if (status == 0) {
        //if the counter for the song is still less than the actual song length
        if (songCounter < currentSongLength) {
            //if chose the first song, initialize the counter that plays through the notes
            //increment the songCounter through the song
            if (currentSongID == 0){
                if (noteCounter >= (joySongNotesTime[songCounter]) * speed) {
                    noteCounter = 0;
                    songCounter++;
                    TA0CCR0 = joyToTheWorldNotes[songCounter]; //up mode is equal to the song notes array
                }
                //increment the notes in the song using the note time array
                else {
                    noteCounter++;
                }
            }
            else {
                //if switch the song to the game of thrones theme
                //increment through the song
                if (noteCounter >= (GoTSongNoteTime[songCounter]) * speed) {
                    // setup next note in the score
                    noteCounter = 0;
                    songCounter++;
                    TA0CCR0 = GoTSongNotes[songCounter];
                }
                //increment the notes in the song using the note time array
                else {
                    noteCounter++;
                }
            }
        }
        
    }
}
ISR_VECTOR(buttonInterrupt,".int02")
ISR_VECTOR(WDT_interval_handler,".int10")
