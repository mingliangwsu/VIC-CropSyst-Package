#include "corn/format/oracle_datasrc.h"
#include "corn/format/datarec.h"
#include "corn/format/vv_entry.h"
#include "corn/string/strconv.h"
#include "corn/condcomp.h"
#include "corn/datetime/date.h"

//#define DEBUG_OCI
#ifdef DEBUG_OCI
#  define DBGOCI
#else
#  define DBGOCI _COMMENT_
#endif
namespace CORN {
//_____________________________________________________________________________/
class Field_conversion_buffer
: public Association
{
 private:
    VV_base_entry *vv_entry;                                                      //010927

 public:
    int         int_buffer;    // int buffer is used for integer types
    char       *str_buffer;    // str_buffer is used for all other types
    sb2         indicator;     // will be -1 if the field was null 0 if not null  //040304
    OCIDefine  *define_handle; // define handle (I don't know where this is allocated or assigned)
    uint16      buffer_size;                                                      //010905
 public:
   inline Field_conversion_buffer(VV_base_entry *_vv_entry,uint16 _buffer_size)  //010928
    : vv_entry(_vv_entry)
    , buffer_size(i_buffer_size)
    , str_buffer(0)
    , int_buffer(0)
    {  unsigned char vv_entry_type = vv_entry->get_type();
       if (!((vv_entry_type == VV_int8) || (vv_entry_type == VV_uint8)
          || (vv_entry_type == VV_int16) || (vv_entry_type == VV_uint16)
          || (vv_entry_type == VV_int32) || (vv_entry_type == VV_uint32)
          || (vv_entry_type == VV_bool)))
       {  str_buffer = new char[i_buffer_size];
          str_buffer[0] = 0;
       }
    }
    inline virtual ~Field_conversion_buffer() { if (str_buffer) delete [] str_buffer; }
    inline void transfer_buffer_to_vv_entry()                                     //010927
    { if (str_buffer && (str_buffer[0] > 0))
          vv_entry->set_str(str_buffer);
      else
       {   char int_str[10];
           CORN_int32_to_str(int_buffer,int_str,10);
           vv_entry->set_str(int_str);
       }
   }
   /*180820 now using key_string 
   inline virtual const char *get_key() const                                      //030117
   { return vv_entry->get_key(); }
   */
};
//_Field_conversion_buffer_____________________________________________________/
class Attribute_name : public Item
{private:
   std::string name;
      // Eventually we may want to add additional info from oracle
 public:
   inline Attribute_name(const char *i_name)
      : Item()
      , name(i_name)
      {}
};
//_Attribute_name______________________________________________________________/
Oracle_server_session::Oracle_server_session(const char *i_user_name,const char *i_password)
: user_name(i_user_name)
, password(i_password)
{  // Create OCI environment, get handles:
   bool created = false;
   // First open the environment all other functions are executed
   // in the context of the environment that is created with this call.
   // OCI_DEFAULT|OCI_OBJECT initialises the mode to be the
   // default mode and allows support for objects.
   // The call returns an environment handle '*envhp' which is
   // then used by the remaining functions.
   created = check_errors(OCIEnvCreate(&envhp, OCI_DEFAULT|OCI_OBJECT,(dvoid *)0,
              (dvoid * (*)(dvoid *, size_t)) 0,(dvoid * (*)(dvoid *, dvoid *, size_t))0,
              (void (*)(dvoid *, dvoid *)) 0,0, (dvoid **)0));
      // OCIHandleAlloc allocates user handles.
      // It returns a pointer to an allocated handle ('*srvhp', '*svchp' '*errhp')
      // corresponding to the type specified ('OCI_HTYPE_SRV',
      // 'OCI_HTYPE_SVCCTX' and 'OCI_HTYPE_ERROR').
      // Handles are allocated with respect to the environment handle
      // '*envhp' which is passed in.
      // Allocate an error handle '*errhp'
   created = check_errors(OCIHandleAlloc ( (dvoid *) envhp, (dvoid **) &errhp,
              OCI_HTYPE_ERROR, (size_t) 0, (dvoid **) 0));
      // Allocate a server handle '*srvhp'
   created = check_errors(OCIHandleAlloc ( (dvoid *) envhp, (dvoid **) &srvhp,
              OCI_HTYPE_SERVER, (size_t) 0, (dvoid **) 0));
      // Allocate a service context handle '*svchp'
   created = check_errors(OCIHandleAlloc ( (dvoid *) envhp, (dvoid **) &svchp,
                       OCI_HTYPE_SVCCTX, (size_t) 0, (dvoid **) 0));
      // OCIServerAttach creates an access path between the data
      // server and the OCI application.
      // It initialises the server handle '*srvhp'.
   created = check_errors(OCIServerAttach (srvhp, errhp, (text *)0,
              (ub4) 0, OCI_DEFAULT));
      // This call sets a particular attribute of a handle.
      // In this case we set the server context attribute
      // 'OCI_ATTR_SERVER' of the service context '*svchp'.
      // 'OCI_HTYPE_SVCCTX' is the service context handle type.
   created = check_errors(OCIAttrSet ( (dvoid *) svchp, OCI_HTYPE_SVCCTX,
          (dvoid *) srvhp, (ub4) 0, OCI_ATTR_SERVER,(OCIError *) errhp));
      // This call allocates and initialises a user session handle
      // corresponding to the type 'OCI_HTYPE_SESSION' specified
      // and returns a pointer '*usrhp' to the handle.
      // The handle is allocated with respect to the environment
      // handle '*envhp' which is passed in.
   created = check_errors(OCIHandleAlloc ( (dvoid *) envhp, (dvoid **)&usrhp,
              OCI_HTYPE_SESSION, (size_t) 0,(dvoid **) 0));
      // This call sets a particular attribute of a handle.
      // In this case we set the user name attribute
      // 'OCI_ATTR_USERNAME' of the user session handle '*usrhp'.
      // 'OCI_HTYPE_SESSION' is the user session handle type.
   created = check_errors(OCIAttrSet ( (dvoid *) usrhp, (ub4) OCI_HTYPE_SESSION,
        (dvoid *) user_name.c_str(), (ub4) user_name.length(),(ub4) OCI_ATTR_USERNAME, errhp));
      //             This call sets a particular attribute of a handle.
      //             In this case we set the user password attribute
      //             'OCI_ATTR_PASSWORD' of the user session handle '*usrhp'.
      //             'OCI_HTYPE_SESSION' is the user session handle type.
   created =  check_errors(OCIAttrSet ( (dvoid *) usrhp, (ub4) OCI_HTYPE_SESSION,
          (dvoid *) password.c_str(), (ub4) password.length(),(ub4) OCI_ATTR_PASSWORD, errhp));
      // Establishes a session for a user (set in *usrhp) on a
      // given server (set in *srvhp).
      // This call is required for the user to execute any operation on the server.
      // Passes in the pointer to the service context handle
      //             '*svchp' and the user session handle pointer '*usrhp'.
      // 'OCI_CRED_RDBMS' indicates we want to authenticate using a
      // DB name and password pair ('OCI_ATTR_PASSWORD' and
      // 'OCI_ATTR_USERNAME' set in '*usrhp') as credentials.
      // OCI_DEFAULT is the mode - the user session context returned
      // may only ever be set with the same server context specified in '*svchp'
   created = check_errors(OCISessionBegin ( svchp, errhp, usrhp, OCI_CRED_RDBMS, (ub4) OCI_DEFAULT));
      //             This call sets a particular attribute of a handle.
      //             In this case we set the user session attribute
      //             'OCI_ATTR_SESSION' of the service context handle '*svchp'.
      //             'OCI_HTYPE_SVCCTX' is the service context handle type.
   created =check_errors(OCIAttrSet ( (dvoid *) svchp, (ub4) OCI_HTYPE_SVCCTX,
            (dvoid *) usrhp, (ub4) 0,(ub4) OCI_ATTR_SESSION, errhp));
}
//_2001-09-05__________________________________________________________________/
Oracle_server_session::~Oracle_server_session()
{  bool terminated = false;
      // Terminate OCI Environment
      // Terminate and delete the user session context created by
      // OCISessionBegin()
   terminated = check_errors(OCISessionEnd(/*(dvoid *)*/ svchp, errhp, usrhp,OCI_DEFAULT));
      //  Delete access to the data source which was established by a
      //  call to OCIServerAttach()
   terminated = check_errors(OCIServerDetach(/*(dvoid *)*/srvhp, errhp, OCI_DEFAULT));
      // Deallocate all handles and storage space allocated to them.
      // Server handle, Error Handle, User Session Handle, Service Context handle,
      // Statement handle and lastly the Environment Handle.
    terminated = check_errors(OCIHandleFree(srvhp, OCI_HTYPE_SERVER));
    terminated = check_errors(OCIHandleFree(errhp, OCI_HTYPE_ERROR));
    terminated = check_errors(OCIHandleFree(usrhp, OCI_HTYPE_SESSION));
    terminated = check_errors(OCIHandleFree(svchp, OCI_HTYPE_SVCCTX));
    terminated = check_errors(OCIHandleFree(envhp, OCI_HTYPE_ENV));
}
//_2001-09-05___________________________________________~Oracle_server_session_/
bool Oracle_server_session::check_errors(sword status)
{  bool ok = false;
   switch (status)
   {  case OCI_SUCCESS: ok = true; /*cerr << "OCI Call Successful" << endl;*/           break;
      case OCI_SUCCESS_WITH_INFO : cerr << "Error - OCI_SUCCESS_WITH_INFO" << endl; break;
      case OCI_NEED_DATA         : cerr << "Error - OCI_NEED_DATA" << endl;         break;
      case OCI_NO_DATA           : cerr << "Error - OCI_NO_DATA" << endl;           break;
      case OCI_ERROR             :
      {  text errbuf[512];
         sb4 errcode = 0;                                                            //010905
         OCIErrorGet ( (dvoid *)errhp, (ub4) 1, (text *) NULL,
               &errcode, errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
         cerr << "Error - " << errbuf << endl;
      } break;
      case OCI_INVALID_HANDLE    : cerr << "Error - OCI_INVALID_HANDLE" << endl;    break;
      case OCI_STILL_EXECUTING   : cerr << "Error - OCI_STILL_EXECUTE" << endl;     break;
      default         : ok = true; cerr << "Warning - OCI Call Unknown return code." << endl; break;
   };
   return ok;
}
//_____________________________________________________________________________/
Oracle_SQL_data_source::Oracle_SQL_data_source
(const char *i_database_name
,const char *i_table_name
,const char *i_primary_key                                                       //021206
, Oracle_server_session &i_server_session)
:Data_source()
,database_name(i_database_name)                                                   //011009
,table_name(i_table_name)
,primary_key(i_primary_key)                                                       //040113
,auto_sequence_primary_key(false)                                                 //040113
,field_conversion_buffers()
,existing_table_attributes()                                                      //010928
,server_session(i_server_session)
,created(false)                                                                   //010920
,statement_handle_pointer(0)                                                      //020914
,always_setup_INSERT_and_SELECT_for_each_record(false)                            //040618
{}
//_____________________________________________________________________________/
void Oracle_SQL_data_source::automatically_sequence_primary_key()
{  auto_sequence_primary_key = true;
}
//_____________________________________________________________________________/
void Oracle_SQL_data_source::order_by(const char *attribute)
{  order_by_attribute.assign(attribute);
}
//_2004-04-01__________________________________________________________________/
Oracle_SQL_data_source::~Oracle_SQL_data_source()
{  free_statement_handle_pointer();
}
//_202-09-14___________________________________________________________________/
bool Oracle_SQL_data_source::load_attributes()
{
    // This retrieves the attribute/column names and data types in the table.
    // Returns true if the table exists and has columns, return false if there are not columns.
    bool error = false;

    CORN::Ustring database_table_name(database_name);
    database_table_name.append(".");
    database_table_name.append(table_name);

    // Get the describe handle for the table:
    OCIDescribe *dschp = 0;
    table_name.to_upper();
    if (check_errors(OCIHandleAlloc((dvoid *)server_session.envhp, (dvoid **) &dschp, (ub4) OCI_HTYPE_DESCRIBE,(size_t) 0, (dvoid **) 0)))
    if (check_errors(OCIDescribeAny(server_session.svchp, server_session.errhp, (dvoid *)database_table_name.c_str(), database_table_name.length(), OCI_OTYPE_NAME, OCI_DEFAULT, OCI_PTYPE_TABLE, dschp)))
    {
       // get the parameter handle
       OCIParam *parmh = 0; // parameter handle
       if (check_errors(OCIAttrGet(dschp, OCI_HTYPE_DESCRIBE, &parmh, 0, OCI_ATTR_PARAM,server_session.errhp)))
       {
          // The type information of the object, in this case, OCI_PTYPE_TABLE,
          // is obtained from the parameter descriptor returned by the OCIAttrGet.
          // Get the number of columns in the table:
          ub2 numcols;
          if (check_errors(OCIAttrGet(parmh, OCI_DTYPE_PARAM, &numcols, 0,OCI_ATTR_NUM_COLS, server_session.errhp)))
          {
             // Get the handle to the column list of the table:
             OCIParam *collsthd = 0; // handle to list of columns
             error = OCIAttrGet(parmh, OCI_DTYPE_PARAM, &collsthd, 0,OCI_ATTR_LIST_COLUMNS, server_session.errhp) == OCI_NO_DATA;
             if (!error)
             {  // Go through the column list and retrieve the data-type of each column,
                // and then describe column types.
                for (ub4 i = 1; i <= numcols; i++)
                {
                   /* get parameter for column i */
                   OCIParam *colhd = 0; /* column handle */
                   if (check_errors(OCIParamGet(collsthd, OCI_DTYPE_PARAM, server_session.errhp,(void **) &colhd, i)))
                   {  char clean_col_name[256];
                      // Retrieve the column name attribute */
                      text *col_name; // RLN it appears that OCIAttrGet returns the pointer
                      ub4 col_name_len; // RLN it appears that this is a value returned by OCIAttrGet
                      check_errors(OCIAttrGet(colhd, /*(ub4)*/ OCI_DTYPE_PARAM
                         ,/*(dvoid**) */&col_name,/*(ub4 *)*/ &col_name_len, (ub4) OCI_ATTR_NAME
                         ,/*(OCIError *)*/ server_session.errhp ));

                      // RLN I don't currently need the data type
                      //check_errors(OCIAttrGet(colhd, OCI_DTYPE_PARAM, &datatype[i-1], 0
                      //,OCI_ATTR_DATA_TYPE, server_session.errhp))
                      strncpy(clean_col_name,(const char *)col_name,col_name_len);
                      clean_col_name[col_name_len] = 0;
                      existing_table_attributes.append(new Attribute_name(/*(const char *)*/clean_col_name));
                   } else error = true;
                }
             }
          } else error = true;
       } else error = true;
    } else error = true;
    else error = true;
    return !error;
}
//_2001-10-10__________________________________________________________________/
void Oracle_SQL_data_source::where_cstr(const char *attribute, const char *value)
{
    // This sets up a where clause to index a field.
    // The attribute column should have unique values.
    get_where_clause.assign(" WHERE ");
    get_where_clause.append(attribute);
    get_where_clause.append(" = '");
    get_where_clause.append(value);
    get_where_clause.append("'");
}
//_____________________________________________________________________________/
void Oracle_SQL_data_source::where_long(const char *attribute, const long value)
{  // This sets up a where clause to index a field.
   // The attribute column should have unique values.
   char value_str[10];
   CORN_int32_to_str(value,value_str,10);
   get_where_clause.assign(" WHERE ");
   get_where_clause.append(attribute);
   get_where_clause.append(" = ");
   get_where_clause.append(value_str);
}
//_____________________________________________________________________________/
void Oracle_SQL_data_source::where2_long
(const char *attribute1, const long value1
,const char *attribute2, const long value2)
{  // This is a where clause with two key attribute
   // This sets up a where clause to index a field.
   // The attribute column should have unique values.
   char value_str1[10];
   CORN_int32_to_str(value1,value_str1,10);
   char value_str2[10];
   CORN_int32_to_str(value2,value_str2,10);
   get_where_clause.assign(" WHERE ");
   get_where_clause.append(attribute1);
   get_where_clause.append(" = ");
   get_where_clause.append(value_str1);
   get_where_clause.append(" AND ");
   get_where_clause.append(attribute2);
   get_where_clause.append(" = ");
   get_where_clause.append(value_str2);
}
//_2002-06-11__________________________________________________________________/
bool Oracle_SQL_data_source::bind_fields(Data_record &data_rec)
{  // binds the fields to the data record.
   int field_index = 1;
   FOR_EACH_IN(section,VV_section,data_rec.sections,each_section)                //030709
   {  FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
      {  //for each entry in current section:
         {  // lookup field in actual fields:
            //const char *entry_label = entry->get_key();                          //030117
            //abandoned std::string entry_label; entry->key_string(entry_label); // 180820 
            Field_conversion_buffer *conversion
                = (Field_conversion_buffer *)field_conversion_buffers.find_string(entry->get_variable() /*180820 entry_label*/,false); //180820
            Attribute_name *existing_attribute
                = (Attribute_name *)existing_table_attributes.find(entry_label,false);  //010928
            if (conversion && existing_attribute)                                //010924
            {  unsigned char entry_type = entry->get_type();
               switch (entry_type)
               {  case VV_bool:                                                  //030214
                  {  check_errors
                      (  OCIDefineByPos
                         ( statement_handle_pointer, &conversion->define_handle, server_session.errhp, field_index
                         ,(dvoid *) &conversion->int_buffer, (int) sizeof(int)
                         ,SQLT_INT
                         ,(dvoid *) &conversion->indicator, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)
                      );
                  } break;
                  case VV_int16 :
                  {  check_errors
                      (  OCIDefineByPos
                         ( statement_handle_pointer, &conversion->define_handle, server_session.errhp, field_index
                         ,(dvoid *) &conversion->int_buffer, (int) sizeof(int)
                         ,SQLT_INT
                         ,(dvoid *) &conversion->indicator, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)   //040304
                      );
                   } break;
                   case VV_int32 :
                   { check_errors
                      (  OCIDefineByPos
                         ( statement_handle_pointer, &conversion->define_handle, server_session.errhp, field_index
                         ,(dvoid *) &conversion->int_buffer, (int32) sizeof(int32)
                         ,SQLT_INT
                         ,(dvoid *) &conversion->indicator, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)   //040304
                      );
                   } break;
                   default:
                   { check_errors
                      (  OCIDefineByPos
                         ( statement_handle_pointer, &conversion->define_handle, server_session.errhp, field_index
                         ,(dvoid *) conversion->str_buffer
                         , conversion->buffer_size
                         ,SQLT_STR
                         ,(dvoid *) &conversion->indicator, (ub2 *) 0, (ub2 *) 0, OCI_DEFAULT)   //040304
                      );
                   } break;
               };
               // needed ? entry->set_encountered(field);
               field_index += 1;                                                 //030213
            }
         }
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return true;
}
//_2002-09-14__________________________________________________________________/
bool Oracle_SQL_data_source::get(Data_record &data_rec)
{  data_rec.get_start();                                                         //021220
   bool got = Data_source::get(data_rec);
//030218    if (!created)   ///  <- Do not delete, eventually this will be restored (see note)

/*030218
Note that normally I shouldn't need to create for each get, but in CropSyst LADSS,
when loading some parameters from different data sources (I.e. Crop parameters)
I need to rebuild the bind_to variable list  I should separate create a function to
reload the variables for this list perhaps if I remember the last data record and check
if there is a different number of fields in the data record.
*/
   create(data_rec); // Make sure we have SELECT statement and field conversion buffers
   // Set up the SELECT statement if we haven't already done so.
   select_from_table_statement.assign(get_SELECT_columns_FROM_table());          //030207
   select_from_table_statement.append(get_where_clause);                         //021218
   if (order_by_attribute.length())                                              //040401
   {  select_from_table_statement.append(" ORDER BY ");                          //040401
      select_from_table_statement.append(order_by_attribute);                    //040401
   };                                                                            //040401
   //causes invalid character error/*030207*/     select_from_table_statement.append(";");
   sword status;            /* for holding return codes */
   /* ********************************************************* */
   // This call allocates and initialises a statement handle
   // corresponding to the type 'OCI_HTYPE_STMT' specified
   // and returns a pointer '*statement_handle_pointer' to the handle.
   //
   // The handle is allocated with respect to the environment
   // handle '*envhp'.
   //
   // When we bind the statement we also need to allocate the
   // storage for the text values we will be using - 'test_str_len'.
   // Sizes are +2 for each to allow for \n and \0.
   //
   // &sel_test_str is the returned pointer to user memory

   //DBGOCI     printf("OCIHandleAlloc...");
    free_statement_handle_pointer();                                              //020914
    check_errors(OCIHandleAlloc ( (dvoid *) server_session.envhp
      , (dvoid **) &statement_handle_pointer,OCI_HTYPE_STMT,0,0));
    //********************************************************* */
    // Request that the SQL 'select_test_table' is prepared for
    // execution using the statement handle 'statement_handle_pointer'.
   //DBGOCI     printf("OCIStmtPrepare...");
   check_errors(OCIStmtPrepare( statement_handle_pointer, server_session.errhp
                  ,(unsigned char *)select_from_table_statement.c_str()
                  ,(ub4) select_from_table_statement.length()
                  ,(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));
   //********************************************************* */
   bind_fields(data_rec);                                                        //020914
   //***************************************************** */
   // Describe the select-list items here, if necessary, using
   // OCIParamGet() and OCIAttrGet().  This is not required if
   // the number of select-list items and the attributes of each
   // item (such as length and datatype) are known at compile time.
   //********************************************************* */
   // Call OCIStmtExecute() to execute the statement.  For DDL
   // statements no further steps are required.
   //
   // 'svchp' is the service context handle pointer.  'statement_handle_pointer'
   // is the statement handle pointer.  OCI_COMMIT_ON_SUCCESS
   // commits the data to the table.
   status = OCIStmtExecute( server_session.svchp, statement_handle_pointer,      //030207
       server_session.errhp, (ub4) 1, (ub4) 0,
                              (CONST OCISnapshot *) NULL,
                              (OCISnapshot *) NULL,
                              OCI_DEFAULT|OCI_COMMIT_ON_SUCCESS);
   got = status != OCI_NO_DATA;                                                  //010928
DBGOCI       if (status == OCI_NO_DATA)
DBGOCI         printf("\n\nNo records found.\n\n");
DBGOCI       else
DBGOCI         check_errors(status);
    // now for each field, we need to update the respective vv_entry in the data record. 010927
   FOR_EACH_IN(conversion,Field_conversion_buffer,field_conversion_buffers,each_conversion) //030709
       conversion->transfer_buffer_to_vv_entry();
   FOR_EACH_END(each_conversion);
//DBGOCI       printf("OCIHandleFree...");

//020914       We now free statement handle pointer either when there are no more records to fetch with get_next(),
//             or a new query is set up, or we are deleted.
//020914       check_errors(  OCIHandleFree(statement_handle_pointer, OCI_HTYPE_STMT));
    data_rec.get_end();                                                           //021220
    return got;
}
//_____________________________________________________________________________/
bool Oracle_SQL_data_source::get_next(Data_record &data_rec)
{  bool got = false;
   if (statement_handle_pointer)
   {  // We must have a statement handle pointer (allocated in first call to get)
      got = Data_source::get_next(data_rec);
      got = bind_fields(data_rec);
      int status = OCIStmtFetch(statement_handle_pointer,server_session.errhp, (ub4) 1, (ub4) OCI_FETCH_NEXT, (ub4)OCI_DEFAULT);
      got = (status == OCI_SUCCESS) || (status < 0); // We accept -1 because this may be NULL column value. //030207
      // now for each field, we need to update the respective vv_entry in the data record.   //010927
      if (got)
      {
         NEW_FOR_EACH_IN(conversion,Field_conversion_buffer,field_conversion_buffers,each_conversion) //030709_
         conversion->transfer_buffer_to_vv_entry();
         FOR_EACH_END(each_conversion);                                             //030709
      }
      else
         free_statement_handle_pointer();
   }
   return got;
}
//_2002-09-14_________________________________Oracle_SQL_data_source::get_next_/
bool Oracle_SQL_data_source::free_statement_handle_pointer()
{  bool success = true;
   if (statement_handle_pointer)
     success = check_errors(  OCIHandleFree(statement_handle_pointer, OCI_HTYPE_STMT));
   statement_handle_pointer = 0;
   return success;
}
//_2002-09-14______________________________________________________________________/
bool Oracle_SQL_data_source::create(Data_record &from_record)
{
    // Currently we are not able to create tables,
    // perhaps eventually we will be able to do this,
    // now we presume the table is created if we can load the attributes.

    created = load_attributes();
    setup_INSERT_and_SELECT_statements(from_record);
    return created;
}
//_2001-09-20___________________________________Oracle_SQL_data_source::create_/
void Oracle_SQL_data_source::setup_INSERT_and_SELECT_statements(Data_record &from_record)
{
   CORN::Ustring insert_column_names;
   CORN::Ustring select_column_names;
   bool need_comma = false;
   FOR_EACH_IN(section,VV_section,from_record.sections,each_section)                //030709
   {                                                                                //______
      FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)                  //030709
      {                                                                             //011010
         ///180820 const char *entry_label = entry->get_key();                                //030117
         //180820 abandoned std::string entry_label; entry->key_string(entry_label); //180820 
         Attribute_name *existing_attribute                                         //011010
         = (Attribute_name *)existing_table_attributes.
            (entry->get_variable() /*180820 entry_label*/,false);     //011010
         if (existing_attribute)                                                    //011010
         {  // lookup field in actual fields:
         // compose column names and placeholders
            if (need_comma)
            {
               insert_column_names.append(",");
               select_column_names.append(",");
            }
            insert_column_names.append(" ");
            select_column_names.append(" ");
            //180820 const char *column_name = entry->get_key();                          //030128
            const std::string &column_name = entry->get_variable(); //180820 
            insert_column_names.append(column_name);
            if (entry->get_type() == VV_date) // Insert and select needs conversion for dates   //010920
            {  select_column_names.append("TO_CHAR("); }
            select_column_names.append(column_name);                             //010920
            if (entry->get_type() == VV_date) // Insert and select needs conversion for dates //010920
            {  select_column_names.append(",'YYYYDDD')"); }
            need_comma = true;
            // Build  field_conversion_buffers
            field_conversion_buffers.append(new Field_conversion_buffer(entry,entry->get_max_strlen()+1));  //010928
         }
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   // Compose the first part of the INSERT INTO statement
   // I now use database.table name for all table names (I haven't checked this)
   INSERT_INTO_columns_VALUES.assign("INSERT INTO ");                               //021218
   INSERT_INTO_columns_VALUES.append(database_name);                                  //030207
   INSERT_INTO_columns_VALUES.append(".");                                            //030207
   INSERT_INTO_columns_VALUES.append(table_name);                                     //021218
   INSERT_INTO_columns_VALUES.append("(");                                            //021218
   INSERT_INTO_columns_VALUES.append(insert_column_names);                            //021218
   INSERT_INTO_columns_VALUES.append(")  VALUES ");                                   //021218

   // Compose the first part of the INSERT INTO statement
   SELECT_columns_FROM_table.assign("SELECT ");                                  //021218
   SELECT_columns_FROM_table.append(select_column_names);                        //021218
   SELECT_columns_FROM_table.append(" FROM ");                                   //021218
   SELECT_columns_FROM_table.append(database_name);                              //030207
   SELECT_columns_FROM_table.append(".");                                        //030207
   SELECT_columns_FROM_table.append(table_name);                                 //021218
}
//_____________________________________________________________________________/
CORN::Ustring Oracle_SQL_data_source::get_INSERT_INTO_columns_VALUES()
{  return INSERT_INTO_columns_VALUES;
}
//_____________________________________________________________________________/
CORN::Ustring Oracle_SQL_data_source::get_SELECT_columns_FROM_table()
{  return SELECT_columns_FROM_table;
}
//_____________________________________________________________________________/
bool Oracle_SQL_data_source::insert(Data_record &data_rec)
{   // Actually this is append (I am using SQL nomenclature)
   bool sat =  check_errors(OCIHandleAlloc ( (dvoid *) server_session.envhp,
         (dvoid **) &statement_handle_pointer,OCI_HTYPE_STMT,0,0));
   CORN::Ustring insert_statement(get_INSERT_INTO_columns_VALUES());             //010924
   // now compose the values list:
   CORN::Ustring values_list("(");                                               //010926
   bool comma_needed = false;                                                    //010926
   FOR_EACH_IN(section,VV_section,data_rec.sections,each_section)                //030709
   {
      //for each entry in current section
      FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)               //030709
      {  // lookup field in actual fields:
         //180820 const char *entry_label = entry->get_key();                             //030117
         const string &entry_label = entry->get_name(); //180820
         // Oracle OCIBindTo needs char array buffers for floats and dates
         // Do the find before the to_upper becase find is case sensitive
         unsigned char vv_type = entry->get_type();
         bool field_encountered_in_table = false;
         Attribute_name *existing_attribute
          = (Attribute_name *)existing_table_attributes.find(entry_label,false); //010928
         if (existing_attribute)
         {  if (comma_needed) values_list.append(",");
             // Some types need to be quoted
             CORN::Ustring item_value;                                           //040113
            switch (vv_type)
            {  case VV_string : case VV_date : case VV_char_array : case VV_Filename : case VV_enum : case VV_unknown :
                  item_value.append("'");                                        //040113
               break;
            }
            switch (vv_type)
            {  case VV_date      :                                               //010926
               {  // We need to make sure dates are in oracle format.
                  VV_date_entry *date_entry = (VV_date_entry *)entry;
                  CORN::Date oracle_formatted_date(date_entry->get_value().get(),CORN_ORACLE_FORMAT); //021218
                  item_value.append(oracle_formatted_date.c_str());              //040113
               } break;
               case VV_bool:                                                     //010926
               {  // Oracle doesn't have a (readily usable) bool type so presume integer.
                  VV_bool_entry *bool_entry = (VV_bool_entry *)entry;
                  bool value = bool_entry->get_value();
                  item_value.append(value?"1":"0");                             //040113
               } break;
               case VV_float32 :                                                //030310
               { char buffer[100];
                  VV_float32_entry *float32_entry = (VV_float32_entry *) entry;
                  strcpy(buffer,CORN::float32_to_str(float32_entry->get_value(),2));
                             // Warning the 2 decimal places is temporary, we should actually load the number from the database.
                  item_value.append(buffer);                                    //040113
               } break;
               default : // All other types are already in a format that Oracle can use
               {  char buffer[100];
                  item_value.append(entry->get(buffer));                        //040113
               } break;
            }
            // Some types need to be quoted
            switch (vv_type)
            { case VV_string : case VV_date : case VV_char_array : case VV_Filename : case VV_enum : case VV_unknown : item_value/*values_list.*/.append("'"); break;
            }
            if ((strcmp(primary_key.c_str(),entry_label) == 0) && auto_sequence_primary_key) //040113
            {  CORN::Ustring table_key_SEQ_NEXT_VAL(database_name);
               table_key_SEQ_NEXT_VAL.append(".");
               table_key_SEQ_NEXT_VAL.append(entry_label);
               table_key_SEQ_NEXT_VAL.append("_SEQ.NEXTVAL");
               item_value.assign(table_key_SEQ_NEXT_VAL);
            }
            values_list.append(item_value);
            comma_needed = true;
         }
         entry->set_encountered(field_encountered_in_table);
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   values_list.append(")");                                                      //010926
   insert_statement.append(values_list);
   sat = check_errors( OCIStmtPrepare( statement_handle_pointer
      ,server_session.errhp, (unsigned char *)insert_statement.c_str()
      ,(ub4) insert_statement.length(),(ub4) OCI_NTV_SYNTAX,(ub4) OCI_DEFAULT));
   sat = check_errors(OCIStmtExecute( server_session.svchp
      , statement_handle_pointer, server_session.errhp, (ub4) 1, (ub4) 0
      ,(CONST OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT|OCI_COMMIT_ON_SUCCESS));
   sat = check_errors(OCIHandleFree(statement_handle_pointer, OCI_HTYPE_STMT));
   return sat;
}
//_____________________________________________________________________________/
bool Oracle_SQL_data_source::update(Data_record &data_rec)
{  // Currently not yet implemented, the LADSS project does not need
   // to update the database.
   return true;
}
//_____________________________________________________________________________/
bool Oracle_SQL_data_source::set_data(Data_record &data_rec)
{  // Currently we presume oracle tables are already setup in their respective database.
   bool result = false;                                                          //050705
   if (!created) create(data_rec);                                               //010920
   if (always_setup_INSERT_and_SELECT_for_each_record)                           //040618
      setup_INSERT_and_SELECT_statements(data_rec);                              //040618
   result = data_rec.get_position()
      // If the record has a record number, we are updating
   ? update(data_rec)
   : insert(data_rec);
   return result;
}
//_2005-07-05_________________________________Oracle_SQL_data_source::set_data_/
const char *Oracle_SQL_data_source::label_string(std::string &buffer) const
{  buffer = table_name;
   return buffer.c_str();
}
//_2017-04-23______________________________Oracle_SQL_data_source::label_string/
bool Oracle_SQL_data_source::delete_records()
{  bool deleted = false;
   deleted = check_errors(OCIHandleAlloc((dvoid *)server_session.envhp,(dvoid **) &statement_handle_pointer
       ,OCI_HTYPE_STMT,0,0));
   CORN::Ustring delete_statement("DELETE FROM ");
   delete_statement.append(database_name);
   delete_statement.append(".");
   delete_statement.append(table_name);
   delete_statement.append(" ");
   delete_statement.append(get_where_clause);
   deleted = check_errors( OCIStmtPrepare( statement_handle_pointer, server_session.errhp, (unsigned char *)delete_statement.c_str(),
                    (ub4) delete_statement.length(),
                    (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));

   deleted = check_errors(OCIStmtExecute( server_session.svchp, statement_handle_pointer, server_session.errhp, (ub4) 1, (ub4) 0,
                   (CONST OCISnapshot *) NULL,
                   (OCISnapshot *) NULL,
                   OCI_DEFAULT|OCI_COMMIT_ON_SUCCESS));
   deleted = free_statement_handle_pointer();                                     //040706
   return deleted;
}
//_2003-03-31____________________________Oracle_SQL_data_source::delete_record_/
}//_namespace_CORN_____________________________________________________________/
