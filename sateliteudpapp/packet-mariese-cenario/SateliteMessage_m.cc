//
// Generated file, do not edit! Created by nedtool 5.6 from apps/sateliteudpapp/packet-mariese-cenario/SateliteMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "SateliteMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

namespace inet {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');

    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(SateliteMessage)

SateliteMessage::SateliteMessage() : ::inet::FieldsChunk()
{
}

SateliteMessage::SateliteMessage(const SateliteMessage& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

SateliteMessage::~SateliteMessage()
{
}

SateliteMessage& SateliteMessage::operator=(const SateliteMessage& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void SateliteMessage::copy(const SateliteMessage& other)
{
    this->tamanhoPacote = other.tamanhoPacote;
    this->portaCarro = other.portaCarro;
    this->clientAddr = other.clientAddr;
    this->tosEscolhido = other.tosEscolhido;
    this->distAntenna = other.distAntenna;
    this->vel = other.vel;
    this->tempoEntrada = other.tempoEntrada;
    this->ri = other.ri;
    this->coordX = other.coordX;
    this->coordY = other.coordY;
    this->id = other.id;
    this->idObj = other.idObj;
    this->tempDetect = other.tempDetect;
    this->obDetect = other.obDetect;
    this->prioridade = other.prioridade;
    this->TempoDeVida = other.TempoDeVida;
    this->obSize = other.obSize;
    this->coordObjX = other.coordObjX;
    this->coordObjY = other.coordObjY;
}

void SateliteMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->tamanhoPacote);
    doParsimPacking(b,this->portaCarro);
    doParsimPacking(b,this->clientAddr);
    doParsimPacking(b,this->tosEscolhido);
    doParsimPacking(b,this->distAntenna);
    doParsimPacking(b,this->vel);
    doParsimPacking(b,this->tempoEntrada);
    doParsimPacking(b,this->ri);
    doParsimPacking(b,this->coordX);
    doParsimPacking(b,this->coordY);
    doParsimPacking(b,this->id);
    doParsimPacking(b,this->idObj);
    doParsimPacking(b,this->tempDetect);
    doParsimPacking(b,this->obDetect);
    doParsimPacking(b,this->prioridade);
    doParsimPacking(b,this->TempoDeVida);
    doParsimPacking(b,this->obSize);
    doParsimPacking(b,this->coordObjX);
    doParsimPacking(b,this->coordObjY);
}

void SateliteMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->tamanhoPacote);
    doParsimUnpacking(b,this->portaCarro);
    doParsimUnpacking(b,this->clientAddr);
    doParsimUnpacking(b,this->tosEscolhido);
    doParsimUnpacking(b,this->distAntenna);
    doParsimUnpacking(b,this->vel);
    doParsimUnpacking(b,this->tempoEntrada);
    doParsimUnpacking(b,this->ri);
    doParsimUnpacking(b,this->coordX);
    doParsimUnpacking(b,this->coordY);
    doParsimUnpacking(b,this->id);
    doParsimUnpacking(b,this->idObj);
    doParsimUnpacking(b,this->tempDetect);
    doParsimUnpacking(b,this->obDetect);
    doParsimUnpacking(b,this->prioridade);
    doParsimUnpacking(b,this->TempoDeVida);
    doParsimUnpacking(b,this->obSize);
    doParsimUnpacking(b,this->coordObjX);
    doParsimUnpacking(b,this->coordObjY);
}

long SateliteMessage::getTamanhoPacote() const
{
    return this->tamanhoPacote;
}

void SateliteMessage::setTamanhoPacote(long tamanhoPacote)
{
    handleChange();
    this->tamanhoPacote = tamanhoPacote;
}

int SateliteMessage::getPortaCarro() const
{
    return this->portaCarro;
}

void SateliteMessage::setPortaCarro(int portaCarro)
{
    handleChange();
    this->portaCarro = portaCarro;
}

const char * SateliteMessage::getClientAddr() const
{
    return this->clientAddr.c_str();
}

void SateliteMessage::setClientAddr(const char * clientAddr)
{
    handleChange();
    this->clientAddr = clientAddr;
}

short SateliteMessage::getTosEscolhido() const
{
    return this->tosEscolhido;
}

void SateliteMessage::setTosEscolhido(short tosEscolhido)
{
    handleChange();
    this->tosEscolhido = tosEscolhido;
}

double SateliteMessage::getDistAntenna() const
{
    return this->distAntenna;
}

void SateliteMessage::setDistAntenna(double distAntenna)
{
    handleChange();
    this->distAntenna = distAntenna;
}

double SateliteMessage::getVel() const
{
    return this->vel;
}

void SateliteMessage::setVel(double vel)
{
    handleChange();
    this->vel = vel;
}

double SateliteMessage::getTempoEntrada() const
{
    return this->tempoEntrada;
}

void SateliteMessage::setTempoEntrada(double tempoEntrada)
{
    handleChange();
    this->tempoEntrada = tempoEntrada;
}

int SateliteMessage::getRi() const
{
    return this->ri;
}

void SateliteMessage::setRi(int ri)
{
    handleChange();
    this->ri = ri;
}

double SateliteMessage::getCoordX() const
{
    return this->coordX;
}

void SateliteMessage::setCoordX(double coordX)
{
    handleChange();
    this->coordX = coordX;
}

double SateliteMessage::getCoordY() const
{
    return this->coordY;
}

void SateliteMessage::setCoordY(double coordY)
{
    handleChange();
    this->coordY = coordY;
}

long SateliteMessage::getId() const
{
    return this->id;
}

void SateliteMessage::setId(long id)
{
    handleChange();
    this->id = id;
}

long SateliteMessage::getIdObj() const
{
    return this->idObj;
}

void SateliteMessage::setIdObj(long idObj)
{
    handleChange();
    this->idObj = idObj;
}

double SateliteMessage::getTempDetect() const
{
    return this->tempDetect;
}

void SateliteMessage::setTempDetect(double tempDetect)
{
    handleChange();
    this->tempDetect = tempDetect;
}

long SateliteMessage::getObDetect() const
{
    return this->obDetect;
}

void SateliteMessage::setObDetect(long obDetect)
{
    handleChange();
    this->obDetect = obDetect;
}

long SateliteMessage::getPrioridade() const
{
    return this->prioridade;
}

void SateliteMessage::setPrioridade(long prioridade)
{
    handleChange();
    this->prioridade = prioridade;
}

long SateliteMessage::getTempoDeVida() const
{
    return this->TempoDeVida;
}

void SateliteMessage::setTempoDeVida(long TempoDeVida)
{
    handleChange();
    this->TempoDeVida = TempoDeVida;
}

long SateliteMessage::getObSize() const
{
    return this->obSize;
}

void SateliteMessage::setObSize(long obSize)
{
    handleChange();
    this->obSize = obSize;
}

double SateliteMessage::getCoordObjX() const
{
    return this->coordObjX;
}

void SateliteMessage::setCoordObjX(double coordObjX)
{
    handleChange();
    this->coordObjX = coordObjX;
}

double SateliteMessage::getCoordObjY() const
{
    return this->coordObjY;
}

void SateliteMessage::setCoordObjY(double coordObjY)
{
    handleChange();
    this->coordObjY = coordObjY;
}

class SateliteMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_tamanhoPacote,
        FIELD_portaCarro,
        FIELD_clientAddr,
        FIELD_tosEscolhido,
        FIELD_distAntenna,
        FIELD_vel,
        FIELD_tempoEntrada,
        FIELD_ri,
        FIELD_coordX,
        FIELD_coordY,
        FIELD_id,
        FIELD_idObj,
        FIELD_tempDetect,
        FIELD_obDetect,
        FIELD_prioridade,
        FIELD_TempoDeVida,
        FIELD_obSize,
        FIELD_coordObjX,
        FIELD_coordObjY,
    };
  public:
    SateliteMessageDescriptor();
    virtual ~SateliteMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(SateliteMessageDescriptor)

SateliteMessageDescriptor::SateliteMessageDescriptor() : omnetpp::cClassDescriptor("inet::SateliteMessage", "inet::FieldsChunk")
{
    propertynames = nullptr;
}

SateliteMessageDescriptor::~SateliteMessageDescriptor()
{
    delete[] propertynames;
}

bool SateliteMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SateliteMessage *>(obj)!=nullptr;
}

const char **SateliteMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *SateliteMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int SateliteMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 19+basedesc->getFieldCount() : 19;
}

unsigned int SateliteMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_tamanhoPacote
        FD_ISEDITABLE,    // FIELD_portaCarro
        FD_ISEDITABLE,    // FIELD_clientAddr
        FD_ISEDITABLE,    // FIELD_tosEscolhido
        FD_ISEDITABLE,    // FIELD_distAntenna
        FD_ISEDITABLE,    // FIELD_vel
        FD_ISEDITABLE,    // FIELD_tempoEntrada
        FD_ISEDITABLE,    // FIELD_ri
        FD_ISEDITABLE,    // FIELD_coordX
        FD_ISEDITABLE,    // FIELD_coordY
        FD_ISEDITABLE,    // FIELD_id
        FD_ISEDITABLE,    // FIELD_idObj
        FD_ISEDITABLE,    // FIELD_tempDetect
        FD_ISEDITABLE,    // FIELD_obDetect
        FD_ISEDITABLE,    // FIELD_prioridade
        FD_ISEDITABLE,    // FIELD_TempoDeVida
        FD_ISEDITABLE,    // FIELD_obSize
        FD_ISEDITABLE,    // FIELD_coordObjX
        FD_ISEDITABLE,    // FIELD_coordObjY
    };
    return (field >= 0 && field < 19) ? fieldTypeFlags[field] : 0;
}

const char *SateliteMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "tamanhoPacote",
        "portaCarro",
        "clientAddr",
        "tosEscolhido",
        "distAntenna",
        "vel",
        "tempoEntrada",
        "ri",
        "coordX",
        "coordY",
        "id",
        "idObj",
        "tempDetect",
        "obDetect",
        "prioridade",
        "TempoDeVida",
        "obSize",
        "coordObjX",
        "coordObjY",
    };
    return (field >= 0 && field < 19) ? fieldNames[field] : nullptr;
}

int SateliteMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 't' && strcmp(fieldName, "tamanhoPacote") == 0) return base+0;
    if (fieldName[0] == 'p' && strcmp(fieldName, "portaCarro") == 0) return base+1;
    if (fieldName[0] == 'c' && strcmp(fieldName, "clientAddr") == 0) return base+2;
    if (fieldName[0] == 't' && strcmp(fieldName, "tosEscolhido") == 0) return base+3;
    if (fieldName[0] == 'd' && strcmp(fieldName, "distAntenna") == 0) return base+4;
    if (fieldName[0] == 'v' && strcmp(fieldName, "vel") == 0) return base+5;
    if (fieldName[0] == 't' && strcmp(fieldName, "tempoEntrada") == 0) return base+6;
    if (fieldName[0] == 'r' && strcmp(fieldName, "ri") == 0) return base+7;
    if (fieldName[0] == 'c' && strcmp(fieldName, "coordX") == 0) return base+8;
    if (fieldName[0] == 'c' && strcmp(fieldName, "coordY") == 0) return base+9;
    if (fieldName[0] == 'i' && strcmp(fieldName, "id") == 0) return base+10;
    if (fieldName[0] == 'i' && strcmp(fieldName, "idObj") == 0) return base+11;
    if (fieldName[0] == 't' && strcmp(fieldName, "tempDetect") == 0) return base+12;
    if (fieldName[0] == 'o' && strcmp(fieldName, "obDetect") == 0) return base+13;
    if (fieldName[0] == 'p' && strcmp(fieldName, "prioridade") == 0) return base+14;
    if (fieldName[0] == 'T' && strcmp(fieldName, "TempoDeVida") == 0) return base+15;
    if (fieldName[0] == 'o' && strcmp(fieldName, "obSize") == 0) return base+16;
    if (fieldName[0] == 'c' && strcmp(fieldName, "coordObjX") == 0) return base+17;
    if (fieldName[0] == 'c' && strcmp(fieldName, "coordObjY") == 0) return base+18;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *SateliteMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "long",    // FIELD_tamanhoPacote
        "int",    // FIELD_portaCarro
        "string",    // FIELD_clientAddr
        "short",    // FIELD_tosEscolhido
        "double",    // FIELD_distAntenna
        "double",    // FIELD_vel
        "double",    // FIELD_tempoEntrada
        "int",    // FIELD_ri
        "double",    // FIELD_coordX
        "double",    // FIELD_coordY
        "long",    // FIELD_id
        "long",    // FIELD_idObj
        "double",    // FIELD_tempDetect
        "long",    // FIELD_obDetect
        "long",    // FIELD_prioridade
        "long",    // FIELD_TempoDeVida
        "long",    // FIELD_obSize
        "double",    // FIELD_coordObjX
        "double",    // FIELD_coordObjY
    };
    return (field >= 0 && field < 19) ? fieldTypeStrings[field] : nullptr;
}

const char **SateliteMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *SateliteMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int SateliteMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    SateliteMessage *pp = (SateliteMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *SateliteMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SateliteMessage *pp = (SateliteMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SateliteMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SateliteMessage *pp = (SateliteMessage *)object; (void)pp;
    switch (field) {
        case FIELD_tamanhoPacote: return long2string(pp->getTamanhoPacote());
        case FIELD_portaCarro: return long2string(pp->getPortaCarro());
        case FIELD_clientAddr: return oppstring2string(pp->getClientAddr());
        case FIELD_tosEscolhido: return long2string(pp->getTosEscolhido());
        case FIELD_distAntenna: return double2string(pp->getDistAntenna());
        case FIELD_vel: return double2string(pp->getVel());
        case FIELD_tempoEntrada: return double2string(pp->getTempoEntrada());
        case FIELD_ri: return long2string(pp->getRi());
        case FIELD_coordX: return double2string(pp->getCoordX());
        case FIELD_coordY: return double2string(pp->getCoordY());
        case FIELD_id: return long2string(pp->getId());
        case FIELD_idObj: return long2string(pp->getIdObj());
        case FIELD_tempDetect: return double2string(pp->getTempDetect());
        case FIELD_obDetect: return long2string(pp->getObDetect());
        case FIELD_prioridade: return long2string(pp->getPrioridade());
        case FIELD_TempoDeVida: return long2string(pp->getTempoDeVida());
        case FIELD_obSize: return long2string(pp->getObSize());
        case FIELD_coordObjX: return double2string(pp->getCoordObjX());
        case FIELD_coordObjY: return double2string(pp->getCoordObjY());
        default: return "";
    }
}

bool SateliteMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    SateliteMessage *pp = (SateliteMessage *)object; (void)pp;
    switch (field) {
        case FIELD_tamanhoPacote: pp->setTamanhoPacote(string2long(value)); return true;
        case FIELD_portaCarro: pp->setPortaCarro(string2long(value)); return true;
        case FIELD_clientAddr: pp->setClientAddr((value)); return true;
        case FIELD_tosEscolhido: pp->setTosEscolhido(string2long(value)); return true;
        case FIELD_distAntenna: pp->setDistAntenna(string2double(value)); return true;
        case FIELD_vel: pp->setVel(string2double(value)); return true;
        case FIELD_tempoEntrada: pp->setTempoEntrada(string2double(value)); return true;
        case FIELD_ri: pp->setRi(string2long(value)); return true;
        case FIELD_coordX: pp->setCoordX(string2double(value)); return true;
        case FIELD_coordY: pp->setCoordY(string2double(value)); return true;
        case FIELD_id: pp->setId(string2long(value)); return true;
        case FIELD_idObj: pp->setIdObj(string2long(value)); return true;
        case FIELD_tempDetect: pp->setTempDetect(string2double(value)); return true;
        case FIELD_obDetect: pp->setObDetect(string2long(value)); return true;
        case FIELD_prioridade: pp->setPrioridade(string2long(value)); return true;
        case FIELD_TempoDeVida: pp->setTempoDeVida(string2long(value)); return true;
        case FIELD_obSize: pp->setObSize(string2long(value)); return true;
        case FIELD_coordObjX: pp->setCoordObjX(string2double(value)); return true;
        case FIELD_coordObjY: pp->setCoordObjY(string2double(value)); return true;
        default: return false;
    }
}

const char *SateliteMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *SateliteMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    SateliteMessage *pp = (SateliteMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

} // namespace inet

