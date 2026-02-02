#ifndef __INET_OBJETO
#define __INET_OBJETO

#include "inet/common/TimeTag_m.h"

#include <iostream>
#include <string>

using namespace omnetpp;
using namespace std;
namespace inet {
    class Objeto {
        public:
            int idObj;
            int ObDetect;
            double coordObjX;
            double coordObjY;
            double obSize;

        Objeto(int id, int ObDetect, double coordObjX,  double coordObjY, double obSize) {
            this->idObj = id;
            this->ObDetect = ObDetect;
            this->coordObjX = coordObjX;
            this->coordObjY = coordObjY;
            this->obSize = obSize;
        }
        int getIdObj() {
            return this->idObj;
        }
        int getObDetect() {
            return this->ObDetect;
        }
        double getcordXOb(){
         return this->coordObjY;
        }
        double getcordYOb(){
            return this->coordObjY;
        }
        double getobSize(){
            return this->obSize;
        }
    };
}

#endif
