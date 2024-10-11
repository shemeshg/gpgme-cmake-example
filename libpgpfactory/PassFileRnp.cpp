#include "PassFileRnp.h"
#include "RnpLoginRequestException.h"

PassFileRnp::PassFileRnp(std::string _fullPath, RnpCoreInterface *rbl)
    : rbl{rbl}
{
    fullPath = _fullPath;
}

void PassFileRnp::_reEncryptFile(std::string pathFileToReEncrypt,
                                std::vector<std::string> encryptTo,
                                bool doSign)
{
        try {
            rbl->reEncryptFile(pathFileToReEncrypt, encryptTo, doSign);
            isMultiThreadError = false;
        } catch (RnpLoginRequestException &rlre) {
            isMultiThreadError = true;   
        } catch (...) {
            throw;
        }

    
}

void PassFileRnp::reEncryptFile(std::string pathFileToReEncrypt,
                                std::vector<std::string> encryptTo,
                                bool doSign)
{
    try {
        rbl->reEncryptFile(pathFileToReEncrypt, encryptTo, doSign);
    }
    catch (RnpLoginRequestException &rlre) {
        isMultiThreadError = true;
        std::lock_guard<std::mutex> lock(mtx);        
        
        while (isMultiThreadError)
        {
            if (rnpPasswdPrompt(rlre)){ 
                _reEncryptFile(pathFileToReEncrypt, encryptTo, doSign);  
            } else {
                throw rlre;
            }                    
        }                        
    } catch (...) {
        throw;
    }
}

void PassFileRnp::encryptFileToFile(std::string fromFileName,
                                    std::string toFileName,
                                    std::vector<std::string> encryptTo,
                                    bool doSign)
{
    rbl->encryptSignFileToFile(fromFileName, toFileName, encryptTo, doSign);
}


void PassFileRnp::encryptStringToFile(std::string s,
                                      std::string toFileName,
                                      std::vector<std::string> encryptTo,
                                      bool doSign)
{
    rbl->encryptSignStringToFile(s, toFileName, encryptTo, doSign);
}

void PassFileRnp::_dectyptFileNameToFileName(std::string fromPath, std::string toPath){
        try {
            rbl->decryptFileToFile(fromPath, toPath);
            isMultiThreadError = false;
        } catch (RnpLoginRequestException &rlre) {
            isMultiThreadError = true;   
        } catch (...) {
            throw;
        }
}
void PassFileRnp::dectyptFileNameToFileName(std::string fromPath, std::string toPath)
{
    try {
        rbl->decryptFileToFile(fromPath, toPath);
    }
    catch (RnpLoginRequestException &rlre) {
        isMultiThreadError = true;
        std::lock_guard<std::mutex> lock(mtx);        
        
        while (isMultiThreadError)
        {
            if (rnpPasswdPrompt(rlre)){ 
                _dectyptFileNameToFileName(fromPath, toPath);     
            } else {
                throw rlre;
            }                  
        }                        
    } catch (...) {
        throw;
    }
}

void PassFileRnp::_decrypt(){
        try {
            rbl->decryptFileToString(fullPath, decrypted, decryptedSignedBy);
            isMultiThreadError = false;
        } catch (RnpLoginRequestException &rlre) {
            isMultiThreadError = true;   
        } catch (...) {
            throw;
        }
}

void PassFileRnp::decrypt()
{

    try {
        rbl->decryptFileToString(fullPath, decrypted, decryptedSignedBy);
    }
    catch (RnpLoginRequestException &rlre) {
        isMultiThreadError = true;
        std::lock_guard<std::mutex> lock(mtx);        
        
        while (isMultiThreadError)
        {
            if (rnpPasswdPrompt(rlre)){ 
                _decrypt();                
            } else {
                throw rlre;
            }    
        }                        
    } catch (...) {
        throw;
    }

}
