#include "../library/library.h"
#include "../library/ReadFileManager.h"				//library è qui dentro
#include "../library/WriteFileManager.h"				//library è qui dentro
#include "../library/EncryptManager.h"
#include "../library/DecryptManager.h"
#include "../library/HMACManager.h"
#include "../library/SendReceiveFile.h"
#include "../library/FileManager.h"
#include "../library/CertificateManager.h"
#include "../library/DHManager.h"
#include "../library/HashManager.h"
#include <iomanip>      // std::setw
#include <limits>

#define CERT_CLIENT_PATH 				"certificates/SSLPClient_cert.pem"
#define SERVER_NAME						"SSLPServer"