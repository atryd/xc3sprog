#include "amiga.h"

#include <libraries/expansion.h>
#include <proto/exec.h>
#include <proto/expansion.h>


#include <iostream>

#define TF_MFG 5080


bool amiga_has_terriblefire_card()
{
    struct ConfigDev *cd;
    bool result = false;

    std::cout << "Checking for TF cards" << std::endl;

    // look for a card with tf manufacturer id
    while ((cd = FindConfigDev(cd, TF_MFG, -1)) != NULL) 
    {
        // need to free?
        std::cout << "Found TF cards" << std::endl;
		result = true;
	}

    return result;
}