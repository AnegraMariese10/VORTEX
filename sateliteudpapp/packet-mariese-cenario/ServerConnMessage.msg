
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

namespace inet;

cplusplus {{
#include "inet/common/packet/Packet.h"
}}
//
// TODO generated message class
//
class ServerConnMessage extends FieldsChunk {
	bool confirmacaoRequest;
    string srcName;
    int portaCarro;
    string clientAddr;
    short tosEscolhido;
    double capacidadeMax = -1;
    long tamanhoPacote = -1;   
    double distAntenna = -1;
    double vel = -1;
    double tempoEntrada = -1;
    int ri = -1;
    double coordX= 0;
    double coordY= 0;
    long tempoCon = -1;
    long videoId;
    long idObj = -1;
    double tempDetect =-1;
    long obDetect = -1;
    long prioridade= -1;
    long TempoDeVida= -1;
    long obSize= -1;
    double coordObjX= 0;
    double coordObjY= 0;
}
