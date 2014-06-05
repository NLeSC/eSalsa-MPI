#ifndef _MACROS_H_
#define _MACROS_H_

#define H2C(IN, OUT) \
OUT = handle_to_communicator(IN);\
if (OUT == NULL) {\
   ERROR(1, "Communicator %d not found!", IN);\
   return EMPI_ERR_COMM;\
}

#define H2T(IN, OUT) \
OUT = handle_to_datatype(IN);\
if (OUT == NULL) {\
   ERROR(1, "Datatype %d not found!", IN);\
   return EMPI_ERR_TYPE;\
}

#define H2R(IN, OUT) \
OUT = handle_to_request(IN);\
if (OUT == NULL) {\
   ERROR(1, "Request %d not found!", IN);\
   return EMPI_ERR_REQUEST;\
}

#define H2O(IN, OUT) \
OUT = handle_to_operation(IN);\
if (OUT == NULL) {\
   ERROR(1, "Operation %d not found!", IN);\
   return EMPI_ERR_OP;\
}

#define H2G(IN, OUT) \
OUT = handle_to_group(IN);\
if (OUT == NULL) {\
   ERROR(1, "Group %d not found!", IN);\
   return EMPI_ERR_OP;\
}

#define H2F(IN, OUT) \
OUT = handle_to_file(IN);\
if (OUT == NULL) {\
   ERROR(1, "File %d not found!", IN);\
   return EMPI_ERR_FILE;\
}

#define H2I(IN, OUT) \
OUT = handle_to_info(IN);\
if (OUT == NULL) {\
   ERROR(1, "Info %d not found!", IN);\
   return EMPI_ERR_INFO;\
}




#endif // _MACROS_H_
