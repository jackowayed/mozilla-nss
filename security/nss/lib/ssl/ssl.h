/*
 * This file contains prototypes for the public SSL functions.
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is the Netscape security libraries.
 * 
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are 
 * Copyright (C) 1994-2000 Netscape Communications Corporation.  All
 * Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 or later (the
 * "GPL"), in which case the provisions of the GPL are applicable 
 * instead of those above.  If you wish to allow use of your 
 * version of this file only under the terms of the GPL and not to
 * allow others to use your version of this file under the MPL,
 * indicate your decision by deleting the provisions above and
 * replace them with the notice and other provisions required by
 * the GPL.  If you do not delete the provisions above, a recipient
 * may use your version of this file under either the MPL or the
 * GPL.
 *
 * $Id$
 */

#ifndef __ssl_h_
#define __ssl_h_

#include "prtypes.h"
#include "prerror.h"
#include "prio.h"
#include "seccomon.h"
#include "cert.h"
#include "keyt.h"

/* constant table enumerating all implemented SSL 2 and 3 cipher suites. */
extern const PRUint16 SSL_ImplementedCiphers[];

/* number of entries in the above table. */
extern const PRUint16 SSL_NumImplementedCiphers;

/* Macro to tell which ciphers in table are SSL2 vs SSL3/TLS. */
#define SSL_IS_SSL2_CIPHER(which) (((which) & 0xfff0) == 0xff00)

SEC_BEGIN_PROTOS


/*
** Imports fd into SSL, returning a new socket.  Copies SSL configuration
** from model.
*/
extern PRFileDesc *SSL_ImportFD(PRFileDesc *model, PRFileDesc *fd);

/*
** Enable/disable an ssl mode
**
** 	SSL_SECURITY:
** 		enable/disable use of SSL security protocol before connect
**
** 	SSL_SOCKS:
** 		enable/disable use of socks before connect
**		(No longer supported).
**
** 	SSL_REQUEST_CERTIFICATE:
** 		require a certificate during secure connect
*/
/* options */
#define SSL_SECURITY			1
#define SSL_SOCKS			2
#define SSL_REQUEST_CERTIFICATE		3
#define SSL_HANDSHAKE_AS_CLIENT		5 /* force accept to hs as client */
#define SSL_HANDSHAKE_AS_SERVER		6 /* force connect to hs as server */
#define SSL_ENABLE_SSL2			7 /* enable ssl v2 (on by default) */
#define SSL_ENABLE_SSL3		        8 /* enable ssl v3 (on by default) */
#define SSL_NO_CACHE		        9 /* don't use the session cache */
#define SSL_REQUIRE_CERTIFICATE        10
#define SSL_ENABLE_FDX                 11 /* permit simultaneous read/write */
#define SSL_V2_COMPATIBLE_HELLO        12 /* send v3 client hello in v2 fmt */
#define SSL_ENABLE_TLS		       13 /* enable TLS (off by default) */

/* Old deprecated function names */
extern SECStatus SSL_Enable(PRFileDesc *fd, int option, PRBool on);
extern SECStatus SSL_EnableDefault(int option, PRBool on);

/* New function names */
extern SECStatus SSL_OptionSet(PRFileDesc *fd, PRInt32 option, PRBool on);
extern SECStatus SSL_OptionGet(PRFileDesc *fd, PRInt32 option, PRBool *on);
extern SECStatus SSL_OptionSetDefault(PRInt32 option, PRBool on);
extern SECStatus SSL_OptionGetDefault(PRInt32 option, PRBool *on);
extern SECStatus SSL_CertDBHandleSet(PRFileDesc *fd, CERTCertDBHandle *dbHandle);

/*
** Control ciphers that SSL uses. If on is non-zero then the named cipher
** is enabled, otherwise it is disabled. 
** The "cipher" values are defined in sslproto.h (the SSL_EN_* values).
** EnableCipher records user preferences.
** SetPolicy sets the policy according to the policy module.
*/
/* Old deprecated function names */
extern SECStatus SSL_EnableCipher(long which, PRBool enabled);
extern SECStatus SSL_SetPolicy(long which, int policy);

/* New function names */
extern SECStatus SSL_CipherPrefSet(PRFileDesc *fd, PRInt32 cipher, PRBool enabled);
extern SECStatus SSL_CipherPrefGet(PRFileDesc *fd, PRInt32 cipher, PRBool *enabled);
extern SECStatus SSL_CipherPrefSetDefault(PRInt32 cipher, PRBool enabled);
extern SECStatus SSL_CipherPrefGetDefault(PRInt32 cipher, PRBool *enabled);
extern SECStatus SSL_CipherPolicySet(PRInt32 cipher, PRInt32 policy);
extern SECStatus SSL_CipherPolicyGet(PRInt32 cipher, PRInt32 *policy);

/* Values for "policy" argument to SSL_PolicySet */
/* Values returned by SSL_CipherPolicyGet. */
#define SSL_NOT_ALLOWED		 0	      /* or invalid or unimplemented */
#define SSL_ALLOWED		 1
#define SSL_RESTRICTED		 2	      /* only with "Step-Up" certs. */

/*
** Reset the handshake state for fd. This will make the complete SSL
** handshake protocol execute from the ground up on the next i/o
** operation.
*/
extern SECStatus SSL_ResetHandshake(PRFileDesc *fd, PRBool asServer);

/*
** Force the handshake for fd to complete immediately.  This blocks until
** the complete SSL handshake protocol is finished.
*/
extern int SSL_ForceHandshake(PRFileDesc *fd);

/*
** Query security status of socket. *on is set to one if security is
** enabled. *keySize will contain the stream key size used. *issuer will
** contain the RFC1485 verison of the name of the issuer of the
** certificate at the other end of the connection. For a client, this is
** the issuer of the server's certificate; for a server, this is the
** issuer of the client's certificate (if any). Subject is the subject of
** the other end's certificate. The pointers can be zero if the desired
** data is not needed.  All strings returned by this function are owned
** by SSL, and will be freed when the socket is closed.
*/
extern int SSL_SecurityStatus(PRFileDesc *fd, int *on, char **cipher,
			      int *keySize, int *secretKeySize,
			      char **issuer, char **subject);

/* Values for "on" */
#define SSL_SECURITY_STATUS_NOOPT	-1
#define SSL_SECURITY_STATUS_OFF		0
#define SSL_SECURITY_STATUS_ON_HIGH	1
#define SSL_SECURITY_STATUS_ON_LOW	2
#define SSL_SECURITY_STATUS_FORTEZZA	3

/*
** Return the certificate for our SSL peer. If the client calls this
** it will always return the server's certificate. If the server calls
** this, it may return NULL if client authentication is not enabled or
** if the client had no certificate when asked.
**	"fd" the socket "file" descriptor
*/
extern CERTCertificate *SSL_PeerCertificate(PRFileDesc *fd);

/*
** Authenticate certificate hook. Called when a certificate comes in
** (because of SSL_REQUIRE_CERTIFICATE in SSL_Enable) to authenticate the
** certificate.
*/
typedef int (*SSLAuthCertificate)(void *arg, PRFileDesc *fd, PRBool checkSig,
				  PRBool isServer);
extern int SSL_AuthCertificateHook(PRFileDesc *fd, SSLAuthCertificate f,
				   void *arg);

/* An implementation of the certificate authentication hook */
extern int SSL_AuthCertificate(void *arg, PRFileDesc *fd, PRBool checkSig,
			       PRBool isServer);

/*
 * Prototype for SSL callback to get client auth data from the application.
 *	arg - application passed argument
 *	caNames - pointer to distinguished names of CAs that the server likes
 *	pRetCert - pointer to pointer to cert, for return of cert
 *	pRetKey - pointer to key pointer, for return of key
 */
typedef int (*SSLGetClientAuthData)(void *arg, PRFileDesc *fd,
				    CERTDistNames *caNames,
				    CERTCertificate **pRetCert,/*return */
				    SECKEYPrivateKey **pRetKey);/* return */

/*
 * Set the client side callback for SSL to retrieve user's private key
 * and certificate.
 *	fd - the file descriptor for the connection in question
 *	f - the application's callback that delivers the key and cert
 *	a - application specific data
 */
extern int SSL_GetClientAuthDataHook(PRFileDesc *fd, SSLGetClientAuthData f,
				     void *a);


/*
 * Set the client side argument for SSL to retrieve PKCS #11 pin.
 *	fd - the file descriptor for the connection in question
 *	a - pkcs11 application specific data
 */
extern int SSL_SetPKCS11PinArg(PRFileDesc *fd, void *a);

/*
** This is a callback for dealing with server certs that are not authenticated
** by the client.  The client app can decide that it actually likes the
** cert by some external means and restart the connection.
*/
typedef int (*SSLBadCertHandler)(void *arg, PRFileDesc *fd);
extern int SSL_BadCertHook(PRFileDesc *fd, SSLBadCertHandler f, void *arg);

/*
** Configure ssl for running a secure server. Needs the
** certificate for the server and the servers private key. The arguments
** are copied.
*/
/* Key Exchange values */
typedef enum {
    kt_null = 0,
    kt_rsa,
    kt_dh,
    kt_fortezza,
    kt_kea_size
} SSLKEAType;

extern SECStatus SSL_ConfigSecureServer(PRFileDesc *fd, CERTCertificate *cert,
				SECKEYPrivateKey *key, SSLKEAType kea);

/*
** Configure a secure servers session-id cache. Define the maximum number
** of entries in the cache, the longevity of the entires, and the directory
** where the cache files will be placed.  These values can be zero, and 
** if so, the implementation will choose defaults.
** This version of the function is for use in applications that have only one 
** process that uses the cache (even if that process has multiple threads).
*/
extern int SSL_ConfigServerSessionIDCache(int      maxCacheEntries,
					  PRUint32 timeout,
					  PRUint32 ssl3_timeout,
				    const char *   directory);
/*
** Like SSL_ConfigServerSessionIDCache, with one important difference.
** If the application will run multiple processes (as opposed to, or in 
** addition to multiple threads), then it must call this function, instead
** of calling SSL_ConfigServerSessionIDCache().
** This has nothing to do with the number of processORs, only processEs.
** This function sets up a Server Session ID (SID) cache that is safe for
** access by multiple processes on the same system.
*/
extern int SSL_ConfigMPServerSIDCache(int      maxCacheEntries, 
				      PRUint32 timeout,
			       	      PRUint32 ssl3_timeout, 
		                const char *   directory);

/* environment variable set by SSL_ConfigMPServerSIDCache, and queried by
 * SSL_InheritMPServerSIDCache when envString is NULL.
 */
#define SSL_ENV_VAR_NAME            "SSL_INHERITANCE"

/* called in child to inherit SID Cache variables. 
 * If envString is NULL, this function will use the value of the environment
 * variable "SSL_INHERITANCE", otherwise the string value passed in will be 
 * used.
 */
extern SECStatus SSL_InheritMPServerSIDCache(const char * envString);

/*
** Set the callback on a particular socket that gets called when we finish
** performing a handshake.
*/
typedef void (*SSLHandshakeCallback)(PRFileDesc *fd, void *client_data);
extern int SSL_HandshakeCallback(PRFileDesc *fd, SSLHandshakeCallback cb,
				 void *client_data);

/*
** For the server, request a new handshake.  For the client, begin a new
** handshake.  If flushCache is non-zero, the SSL3 cache entry will be 
** flushed first, ensuring that a full SSL handshake will be done.
** If flushCache is zero, and an SSL connection is established, it will 
** do the much faster session restart handshake.  This will change the 
** session keys without doing another private key operation.
*/
extern int SSL_ReHandshake(PRFileDesc *fd, PRBool flushCache);

/*
** For the server, request a new handshake.  For the client, begin a new
** handshake.  Flushes SSL3 session cache entry first, ensuring that a 
** full handshake will be done.  
** This call is equivalent to SSL_ReHandshake(fd, PR_TRUE)
*/
extern int SSL_RedoHandshake(PRFileDesc *fd);

/*
** Return 1 if the socket is direct, 0 if not, -1 on error
*/
extern int SSL_CheckDirectSock(PRFileDesc *s);

/*
** A cousin to SSL_Bind, this takes an extra arg: dsthost, so we can
** set up sockd connection. This should be used with socks enabled.
*/
extern int SSL_BindForSockd(PRFileDesc *s, PRNetAddr *sa, long dsthost);

/*
** Configure ssl for using socks.
*/
extern SECStatus SSL_ConfigSockd(PRFileDesc *fd, PRUint32 host, PRUint16 port);

/*
 * Allow the application to pass a URL or hostname into the SSL library
 */
extern int SSL_SetURL(PRFileDesc *fd, const char *url);

/*
** Return the number of bytes that SSL has waiting in internal buffers.
** Return 0 if security is not enabled.
*/
extern int SSL_DataPending(PRFileDesc *fd);

/*
** Invalidate the SSL session associated with fd.
*/
extern int SSL_InvalidateSession(PRFileDesc *fd);

/*
** Return a SECItem containing the SSL session ID associated with the fd.
*/
extern SECItem *SSL_GetSessionID(PRFileDesc *fd);

/*
** Clear out the SSL session cache.
*/
extern void SSL_ClearSessionCache(void);

/*
** Set peer information so we can correctly look up SSL session later.
** You only have to do this if you're tunneling through a proxy.
*/
extern int SSL_SetSockPeerID(PRFileDesc *fd, char *peerID);

/*
** Read the socks config file.  You must do this before doing anything with
** socks.
*/
extern int SSL_ReadSocksConfFile(PRFileDesc *fp);

/*
** Reveal the security information for the peer. 
*/
extern CERTCertificate * SSL_RevealCert(PRFileDesc * socket);
extern void * SSL_RevealPinArg(PRFileDesc * socket);
extern char * SSL_RevealURL(PRFileDesc * socket);


/* This callback may be passed to the SSL library via a call to
 * SSL_GetClientAuthDataHook() for each SSL client socket.
 * It will be invoked when SSL needs to know what certificate and private key
 * (if any) to use to respond to a request for client authentication.
 * If arg is non-NULL, it is a pointer to a NULL-terminated string containing
 * the nickname of the cert/key pair to use.
 * If arg is NULL, this function will search the cert and key databases for 
 * a suitable match and send it if one is found.
 */
extern SECStatus
NSS_GetClientAuthData(void *                       arg,
                      PRFileDesc *                 socket,
                      struct CERTDistNamesStr *    caNames,
                      struct CERTCertificateStr ** pRetCert,
                      struct SECKEYPrivateKeyStr **pRetKey);

/*
 * Look to see if any of the signers in the cert chain for "cert" are found
 * in the list of caNames.  
 * Returns SECSuccess if so, SECFailure if not.
 * Used by NSS_GetClientAuthData.  May be used by other callback functions.
 */
extern SECStatus NSS_CmpCertChainWCANames(CERTCertificate *cert, 
                                          CERTDistNames *caNames);

/* 
 * Returns key exchange type of the keys in an SSL server certificate.
 */
extern SSLKEAType NSS_FindCertKEAType(CERTCertificate * cert);

/* Set cipher policies to a predefined Domestic (U.S.A.) policy.
 * This essentially enables all supported ciphers.
 */
extern SECStatus NSS_SetDomesticPolicy(void);

/* Set cipher policies to a predefined Policy that is exportable from the USA
 *   according to present U.S. policies as we understand them.
 * See documentation for the list.
 * Note that your particular application program may be able to obtain
 *   an export license with more or fewer capabilities than those allowed
 *   by this function.  In that case, you should use SSL_SetPolicy()
 *   to explicitly allow those ciphers you may legally export.
 */
extern SECStatus NSS_SetExportPolicy(void);

/* Set cipher policies to a predefined Policy that is exportable from the USA
 *   according to present U.S. policies as we understand them, and that the 
 *   nation of France will permit to be imported into their country.
 * See documentation for the list.
 */
extern SECStatus NSS_SetFrancePolicy(void);

SEC_END_PROTOS

#endif /* __ssl_h_ */
