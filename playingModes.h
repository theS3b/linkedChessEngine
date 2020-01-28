#pragma once
#include "socket_connexion_python.h"
#define	DEPTH	5
#define BADMOVE	"BADMOVE"
#define LOST	"LO"
#define WIN		"WI"

bool playingAgainstAi(SocketConnexion & conn);
bool AIvsAI();