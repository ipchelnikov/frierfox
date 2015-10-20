#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <string.h>

#include "Friersock.h"
#include "logger.h"

using namespace std;

mutex command_mutex;
mutex ready_mutex;
condition_variable cv_command_ready;

queue<string> commands;

Friersock *frier = NULL;

void command_handler() 
{
    char   psBuffer[128];
    FILE   *pPipe;

    while ( true )
    {
        string cur_command;

        command_mutex.lock();
        if (commands.empty()) 
        {
            command_mutex.unlock();

            unique_lock<std::mutex> lck(ready_mutex);
            TRACE << "Waiting for command..." << ENDL;
            cv_command_ready.wait(lck);

            command_mutex.lock();
        }
        
        cur_command = commands.front();
        commands.pop();
        command_mutex.unlock();
        
        TRACE << "Handling command: " << cur_command.c_str() << ENDL;

        pPipe = _popen(cur_command.c_str(), "rt");

        if (pPipe == NULL)
        {
            TRACE << "pPipe returned an error" << ENDL;
            continue;
        }

        // Read pipe until end of file, or an error occurs.

        while (fgets(psBuffer, 128, pPipe))
        {
            TRACE << "Sending buf: " << psBuffer << ENDL;

            // Fixing cmd output TODO: UDTF-16?
            for (int i = 0; i < 128; i++)
                if (psBuffer[i] == -1)
                    psBuffer[i] = ' ';


            // TODO Lock
            frier->send(psBuffer);
        }

        if (feof(pPipe))
        {
            TRACE << "Process returned: " << _pclose(pPipe) << ENDL;
        }
        else
        {
            TRACE << "Error: Failed to read the pipe to the end" << ENDL;
        }
    }
}

int main(int argc, char *argv[])
{
    thread command_thread(command_handler);
    
    while (true)
    {
        TRACE << "Init..." << ENDL;

        frier = new Friersock(); // TODO smart pointer
        if (frier->init() != 0) 
        {
            TRACE << "Fatal error on socket initialization. Exit..." << ENDL;
            exit(1);
        }

        while (frier->listen() > 0)
        {
            command_mutex.lock();
            commands.push(string(frier->getBuf()));
            command_mutex.unlock();

            // TODO: Check if command_handler still running

            cv_command_ready.notify_one();
        }

        delete frier;
    }


    TRACE << "Exit..." << ENDL;

    cin.ignore();

    return 0; 
}