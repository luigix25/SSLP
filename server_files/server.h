#include "../library/library.h"
#include "../library/ReadFileManager.h"				//library è qui dentro
#include "../library/EncryptManager.h"
#include "../library/HMACManager.h"
#include "../library/CertificateManager.h"
#include "../library/SendReceiveFile.h"
#include "../library/RSAEncryptManager.h"
#include "../library/DHManager.h"
#include "../library/HashManager.h"



#define CERT_SERVER_PATH 				"certificates/SSLPServer_cert.pem"
#define CLIENTS_LIST					"server_files/clients.txt"