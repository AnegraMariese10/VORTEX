
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

import inet.common.packet.Packet;
//import inet.common.geometry.common.Coord;

namespace inet;

cplusplus {{
#include "inet/common/packet/Packet.h"
#include "inet/common/geometry/common/Coord.h"
}}
//
// TODO generated message class
//
class SateliteMessage extends FieldsChunk {
    long tamanhoPacote;
    int portaCarro;
    string clientAddr;
    short tosEscolhido;
    double distAntenna;
    double vel;
    double tempoEntrada;
    int ri;
    double coordX;
    double coordY;
    long id;    
    long idObj;
    double tempDetect;
    long obDetect;
    long prioridade;
    long TempoDeVida;
    long obSize;
    double coordObjX;
    double coordObjY;
}
