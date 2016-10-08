# SecureCodingParser

Program expects one input parameter - path to the ZIP file.

Parser expects PK format of ZIP - https://users.cs.jmu.edu/buchhofp/forensics/formats/pkzip.html

Official PK ZIP spec - https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT

## PK Zip structure

    PKZIP_FILE := FILE_SECTION* CENTRAL_DIR END_HEADERS
    
    FILE_SECTION := local_file_header [encryption_header] [file_data] [data_descriptor]
    
    CENTRAL_DIR :=  [archive decryption header] [archive extra data record] [central_directory_header]* [digital signature]
    
    END_HEADERS := [zip64_end_of_central_directory_record] [zip64_end_of_central_directory_locator] end_of_central_directory_record

end_of_central_directory_record is the only mandatory section