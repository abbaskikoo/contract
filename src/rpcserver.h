// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPCSERVER_H
#define BITCOIN_RPCSERVER_H

#include "amount.h"
#include "rpcprotocol.h"
#include "uint256.h"

#include <list>
#include <map>
#include <stdint.h>
#include <string>

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_utils.h"
#include "json/json_spirit_writer_template.h"

class CBlockIndex;
class CNetAddr;

class AcceptedConnection
{
public:
    virtual ~AcceptedConnection() {}

    virtual std::iostream& stream() = 0;
    virtual std::string peer_address_to_string() const = 0;
    virtual void close() = 0;
};

/** Start RPC threads */
void StartRPCThreads();
/**
 * Alternative to StartRPCThreads for the GUI, when no server is
 * used. The RPC thread in this case is only used to handle timeouts.
 * If real RPC threads have already been started this is a no-op.
 */
void StartDummyRPCThread();
/** Stop RPC threads */
void StopRPCThreads();
/** Query whether RPC is running */
bool IsRPCRunning();

/** 
 * Set the RPC warmup status.  When this is done, all RPC calls will error out
 * immediately with RPC_IN_WARMUP.
 */
void SetRPCWarmupStatus(const std::string& newStatus);
/* Mark warmup as done.  RPC calls will be processed from now on.  */
void SetRPCWarmupFinished();

/* returns the current warmup state.  */
bool RPCIsInWarmup(std::string *statusOut);

/**
 * Type-check arguments; throws JSONRPCError if wrong type given. Does not check that
 * the right number of arguments are passed, just that any passed are the correct type.
 * Use like:  RPCTypeCheck(params, boost::assign::list_of(str_type)(int_type)(obj_type));
 */
void RPCTypeCheck(const json_spirit::Array& params,
                  const std::list<json_spirit::Value_type>& typesExpected, bool fAllowNull=false);
/**
 * Check for expected keys/value types in an Object.
 * Use like: RPCTypeCheck(object, boost::assign::map_list_of("name", str_type)("value", int_type));
 */
void RPCTypeCheck(const json_spirit::Object& o,
                  const std::map<std::string, json_spirit::Value_type>& typesExpected, bool fAllowNull=false);

/**
 * Run func nSeconds from now. Uses boost deadline timers.
 * Overrides previous timer <name> (if any).
 */
void RPCRunLater(const std::string& name, boost::function<void(void)> func, int64_t nSeconds);

//! Convert boost::asio address to CNetAddr
extern CNetAddr BoostAsioToCNetAddr(boost::asio::ip::address address);

typedef json_spirit::Value(*rpcfn_type)(const json_spirit::Array& params, bool fHelp);

class CRPCCommand
{
public:
    std::string category;
    std::string name;
    rpcfn_type actor;
    bool okSafeMode;
    bool threadSafe;
    bool reqWallet;
};

/**
 * Bitcoin RPC command dispatcher.
 */
class CRPCTable
{
private:
    std::map<std::string, const CRPCCommand*> mapCommands;
public:
    CRPCTable();
    const CRPCCommand* operator[](std::string name) const;
    std::string help(std::string name) const;

    /**
     * Execute a method.
     * @param method   Method to execute
     * @param params   Array of arguments (JSON objects)
     * @returns Result of the call.
     * @throws an exception (json_spirit::Value) when an error happens.
     */
    json_spirit::Value execute(const std::string &method, const json_spirit::Array &params) const;
};

extern const CRPCTable tableRPC;

/**
 * Utilities: convert hex-encoded Values
 * (throws error if not hex).
 */
extern uint256 ParseHashV(const json_spirit::Value& v, std::string strName);
extern uint256 ParseHashO(const json_spirit::Object& o, std::string strKey);
extern std::vector<unsigned char> ParseHexV(const json_spirit::Value& v, std::string strName);
extern std::vector<unsigned char> ParseHexO(const json_spirit::Object& o, std::string strKey);

extern void InitRPCMining();
extern void ShutdownRPCMining();

extern int64_t nWalletUnlockTime;
extern CAmount AmountFromValue(const json_spirit::Value& value);
extern json_spirit::Value ValueFromAmount(const CAmount& amount);
extern double GetDifficulty(const CBlockIndex* blockindex = NULL);
extern std::string HelpRequiringPassphrase();
extern std::string HelpExampleCli(std::string methodname, std::string args);
extern std::string HelpExampleRpc(std::string methodname, std::string args);

extern void EnsureWalletIsUnlocked();

extern json_spirit::Value getconnectioncount(const json_spirit::Array& params, bool fHelp); // in rpcnet.cpp
extern json_spirit::Value getpeerinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value ping(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value addnode(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getaddednodeinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getnettotals(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value dumpprivkey(const json_spirit::Array& params, bool fHelp); // in rpcdump.cpp
extern json_spirit::Value importprivkey(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value importaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value dumpwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value importwallet(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getgenerate(const json_spirit::Array& params, bool fHelp); // in rpcmining.cpp
extern json_spirit::Value setgenerate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getnetworkhashps(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gethashespersec(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getmininginfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value prioritisetransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblocktemplate(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value submitblock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value estimatefee(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value estimatepriority(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getnewaddress(const json_spirit::Array& params, bool fHelp); // in rpcwallet.cpp
extern json_spirit::Value getaccountaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getrawchangeaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value setaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getaddressesbyaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendtoaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value signmessage(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value verifymessage(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getreceivedbyaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getreceivedbyaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getbalance(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getunconfirmedbalance(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value movecmd(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendfrom(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendmany(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value addmultisigaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value createmultisig(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listreceivedbyaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listreceivedbyaccount(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listtransactions(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listaddressgroupings(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listaccounts(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listsinceblock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value backupwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value keypoolrefill(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletpassphrase(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletpassphrasechange(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value walletlock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value encryptwallet(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value validateaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getwalletinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblockchaininfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getnetworkinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value setmocktime(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getrawtransaction(const json_spirit::Array& params, bool fHelp); // in rcprawtransaction.cpp
extern json_spirit::Value listunspent(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value lockunspent(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listlockunspent(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value createrawtransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value decoderawtransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value decodescript(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value signrawtransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendrawtransaction(const json_spirit::Array& params, bool fHelp);

extern json_spirit::Value getblockcount(const json_spirit::Array& params, bool fHelp); // in rpcblockchain.cpp
extern json_spirit::Value getbestblockhash(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getdifficulty(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value settxfee(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getmempoolinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getrawmempool(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value clearmempool(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblockhash(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getblock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettxoutsetinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettxout(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value verifychain(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getchaintips(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value invalidateblock(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value reconsiderblock(const json_spirit::Array& params, bool fHelp);

/* Omni Core data retrieval calls */
extern json_spirit::Value omni_getinfo(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getactivations(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getallbalancesforid(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getbalance(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_gettransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_listtransactions(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getproperty(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_listproperties(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getcrowdsale(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getgrants(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getactivedexsells(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getactivecrowdsales(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getorderbook(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_gettrade(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getsto(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_listblocktransactions(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_listpendingtransactions(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getallbalancesforaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_gettradehistoryforaddress(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_gettradehistoryforpair(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getcurrentconsensushash(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getpayload(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getseedblocks(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getmetadexhash(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getfeecache(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getfeetrigger(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getfeeshare(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getfeedistribution(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_getfeedistributions(const json_spirit::Array& params, bool fHelp);

/* Omni Core configuration calls */
extern json_spirit::Value omni_setautocommit(const json_spirit::Array& params, bool fHelp);

/* Omni Core transaction calls */
extern json_spirit::Value omni_sendrawtx(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_send(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_senddexsell(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_senddexaccept(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendissuancecrowdsale(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendissuancefixed(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendissuancemanaged(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendtrade(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendcanceltradesbyprice(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendcanceltradesbypair(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendcancelalltrades(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendsto(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendgrant(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendrevoke(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendclosecrowdsale(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendchangeissuer(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendall(const json_spirit::Array& params, bool fHelp);

/* Omni Core payload creation calls */
extern json_spirit::Value omni_createpayload_simplesend(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_sendall(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_dexsell(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_dexaccept(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_sto(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_issuancefixed(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_issuancecrowdsale(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_issuancemanaged(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_closecrowdsale(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_grant(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_revoke(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_changeissuer(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_trade(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_canceltradesbyprice(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_canceltradesbypair(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createpayload_cancelalltrades(const json_spirit::Array& params, bool fHelp);

/* Omni Core hidden calls - development usage (not shown in help) */
extern json_spirit::Value mscrpc(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_senddeactivation(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendactivation(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_sendalert(const json_spirit::Array& params, bool fHelp);

/* Omni Core raw transaction calls */
extern json_spirit::Value omni_decodetransaction(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createrawtx_opreturn(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createrawtx_multisig(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createrawtx_input(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createrawtx_reference(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value omni_createrawtx_change(const json_spirit::Array& params, bool fHelp);

/* Omni Core hidden calls - aliased calls for backwards compatibiltiy - to be depreciated (not shown in help) */
extern json_spirit::Value trade_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getinfo_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getallbalancesforid_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getbalance_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value send_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettransaction_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listtransactions_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getproperty_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listproperties_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getcrowdsale_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getgrants_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getactivedexsells_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getactivecrowdsales_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getorderbook_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendtoowners_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value sendrawtx_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getsto_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value gettrade_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value listblocktransactions_MP(const json_spirit::Array& params, bool fHelp);
extern json_spirit::Value getallbalancesforaddress_MP(const json_spirit::Array& params, bool fHelp);

// in rest.cpp
extern bool HTTPReq_REST(AcceptedConnection *conn,
                  std::string& strURI,
                  std::map<std::string, std::string>& mapHeaders,
                  bool fRun);

#endif // BITCOIN_RPCSERVER_H