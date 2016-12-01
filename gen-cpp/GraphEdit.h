/**
 * Autogenerated by Thrift Compiler (0.9.3)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef GraphEdit_H
#define GraphEdit_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include "rpc_types.h"

namespace rpc {

#ifdef _WIN32
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class GraphEditIf {
 public:
  virtual ~GraphEditIf() {}
  virtual bool editGraph(const Packet& p) = 0;
};

class GraphEditIfFactory {
 public:
  typedef GraphEditIf Handler;

  virtual ~GraphEditIfFactory() {}

  virtual GraphEditIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(GraphEditIf* /* handler */) = 0;
};

class GraphEditIfSingletonFactory : virtual public GraphEditIfFactory {
 public:
  GraphEditIfSingletonFactory(const boost::shared_ptr<GraphEditIf>& iface) : iface_(iface) {}
  virtual ~GraphEditIfSingletonFactory() {}

  virtual GraphEditIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(GraphEditIf* /* handler */) {}

 protected:
  boost::shared_ptr<GraphEditIf> iface_;
};

class GraphEditNull : virtual public GraphEditIf {
 public:
  virtual ~GraphEditNull() {}
  bool editGraph(const Packet& /* p */) {
    bool _return = false;
    return _return;
  }
};

typedef struct _GraphEdit_editGraph_args__isset {
  _GraphEdit_editGraph_args__isset() : p(false) {}
  bool p :1;
} _GraphEdit_editGraph_args__isset;

class GraphEdit_editGraph_args {
 public:

  GraphEdit_editGraph_args(const GraphEdit_editGraph_args&);
  GraphEdit_editGraph_args& operator=(const GraphEdit_editGraph_args&);
  GraphEdit_editGraph_args() {
  }

  virtual ~GraphEdit_editGraph_args() throw();
  Packet p;

  _GraphEdit_editGraph_args__isset __isset;

  void __set_p(const Packet& val);

  bool operator == (const GraphEdit_editGraph_args & rhs) const
  {
    if (!(p == rhs.p))
      return false;
    return true;
  }
  bool operator != (const GraphEdit_editGraph_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const GraphEdit_editGraph_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class GraphEdit_editGraph_pargs {
 public:


  virtual ~GraphEdit_editGraph_pargs() throw();
  const Packet* p;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _GraphEdit_editGraph_result__isset {
  _GraphEdit_editGraph_result__isset() : success(false) {}
  bool success :1;
} _GraphEdit_editGraph_result__isset;

class GraphEdit_editGraph_result {
 public:

  GraphEdit_editGraph_result(const GraphEdit_editGraph_result&);
  GraphEdit_editGraph_result& operator=(const GraphEdit_editGraph_result&);
  GraphEdit_editGraph_result() : success(0) {
  }

  virtual ~GraphEdit_editGraph_result() throw();
  bool success;

  _GraphEdit_editGraph_result__isset __isset;

  void __set_success(const bool val);

  bool operator == (const GraphEdit_editGraph_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const GraphEdit_editGraph_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const GraphEdit_editGraph_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _GraphEdit_editGraph_presult__isset {
  _GraphEdit_editGraph_presult__isset() : success(false) {}
  bool success :1;
} _GraphEdit_editGraph_presult__isset;

class GraphEdit_editGraph_presult {
 public:


  virtual ~GraphEdit_editGraph_presult() throw();
  bool* success;

  _GraphEdit_editGraph_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class GraphEditClient : virtual public GraphEditIf {
 public:
  GraphEditClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  GraphEditClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  bool editGraph(const Packet& p);
  void send_editGraph(const Packet& p);
  bool recv_editGraph();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class GraphEditProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<GraphEditIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (GraphEditProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_editGraph(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  GraphEditProcessor(boost::shared_ptr<GraphEditIf> iface) :
    iface_(iface) {
    processMap_["editGraph"] = &GraphEditProcessor::process_editGraph;
  }

  virtual ~GraphEditProcessor() {}
};

class GraphEditProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  GraphEditProcessorFactory(const ::boost::shared_ptr< GraphEditIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< GraphEditIfFactory > handlerFactory_;
};

class GraphEditMultiface : virtual public GraphEditIf {
 public:
  GraphEditMultiface(std::vector<boost::shared_ptr<GraphEditIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~GraphEditMultiface() {}
 protected:
  std::vector<boost::shared_ptr<GraphEditIf> > ifaces_;
  GraphEditMultiface() {}
  void add(boost::shared_ptr<GraphEditIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  bool editGraph(const Packet& p) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->editGraph(p);
    }
    return ifaces_[i]->editGraph(p);
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
class GraphEditConcurrentClient : virtual public GraphEditIf {
 public:
  GraphEditConcurrentClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
    setProtocol(prot);
  }
  GraphEditConcurrentClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    setProtocol(iprot,oprot);
  }
 private:
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) {
  setProtocol(prot,prot);
  }
  void setProtocol(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  bool editGraph(const Packet& p);
  int32_t send_editGraph(const Packet& p);
  bool recv_editGraph(const int32_t seqid);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
  ::apache::thrift::async::TConcurrentClientSyncInfo sync_;
};

#ifdef _WIN32
  #pragma warning( pop )
#endif

} // namespace

#endif
