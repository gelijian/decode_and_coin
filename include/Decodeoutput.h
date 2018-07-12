#ifndef DECODEOUTPUT_H
#define DECODEOUTPUT_H
class Decodeoutput
{
    public:
        Decodeoutput();
        void SetValue(char* datapath, int ch);
        void CloseFile(){fclose(pointer_file);}
	public:
		char filename[500];
		FILE* pointer_file ;
        int channel;
};

#endif
