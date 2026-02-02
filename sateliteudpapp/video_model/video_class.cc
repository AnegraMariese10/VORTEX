#ifndef __INET_VIDEO
#define __INET_VIDEO

#include "inet/common/TimeTag_m.h"

#include <iostream>
#include <string>

using namespace omnetpp;
using namespace std;
namespace inet {
    class Video {
        public:
            int id;
            int prioridade;
            double tamanho;
            double ttl; // (time to live) segundos
            double timeCriacao;
            double deadline;
            double coordX;
            double coordY;
            int idObj;
            int obDetect;
            double obSize;
            double coordObjX;
            double coordObjY;


            Video(int id, double timeCriacao, int prioridade, double tamanho, double ttl, double coordX, double coordY, int idObj, int obDetect,double objSize, double coordObjX, double coordObjY){
                this->id = id;
                this->prioridade = prioridade;
                this->tamanho = tamanho;
                this->ttl = ttl;
                this->coordX = coordX;
                this->coordY = coordY;
                this->idObj = idObj;
                this->obDetect = obDetect;
                this->obSize = objSize;
                this->coordObjX = coordObjX;
                this->coordObjY = coordObjY;
                this->timeCriacao = simTime().dbl();
                this->deadline = this->timeCriacao + this->ttl;
            }

            Video() {
                this->id = -1;
                this->prioridade = 3;
                this->tamanho = -1;
                this->ttl = -1;
                this->timeCriacao = -1;
                this->deadline = -1;
                this->idObj = -1;
                this->obDetect = -1;
                this->obSize = -1;
                this->coordObjX = -1;
                this->coordObjY = -1;
            }

            int getId() {
                return this->id;
            }

            double getDeadline() {
                return this->deadline;
            }

            int getPrioridade() {
                return this->prioridade;
            }

            double getTamanho() {
                return this->tamanho;
            }

            double getTimeCriacao(){
                return this->timeCriacao;
            }
            double getCoordX() {
                return this->coordX;
            }
            double getCoordY() {
               return this->coordY;
            }

            int getIdObj() {
                return this->idObj;
            }
            int getobDetect() {
                return this->obDetect;
            }

            double getobSize(){
                return this->obSize;
            }
            double getCoordObjX() {
                return this->coordObjX;
            }
            double getCoordObjY() {
               return this->coordObjY;
            }
    };
}

#endif // ifndef __INET_UDPVIDEOSTREAMCLI_H
