

        /**
         * HadoopStopResult.cpp
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */
        
            #include "AviaryHadoop_HadoopStopResult.h"
          

       #ifdef __GNUC__
       # if __GNUC__ >= 4
       #pragma GCC diagnostic ignored "-Wcast-qual"
       #pragma GCC diagnostic ignored "-Wshadow"
       #pragma GCC diagnostic ignored "-Wunused-parameter"
       #pragma GCC diagnostic ignored "-Wunused-variable"
       #pragma GCC diagnostic ignored "-Wunused-value"
       #pragma GCC diagnostic ignored "-Wwrite-strings"
       #  if __GNUC_MINOR__ >= 6
       #pragma GCC diagnostic ignored "-Wenum-compare"
       #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
       #pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
       #  endif
       #  if __GNUC_MINOR__ >= 7
       #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
       #  endif
       # endif
       #endif
        
        #include <Environment.h>
        #include <WSFError.h>


        using namespace wso2wsf;
        using namespace std;
        
        using namespace AviaryHadoop;
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = HadoopStopResult
                 * Namespace URI = http://hadoop.aviary.grid.redhat.com
                 * Namespace Prefix = ns2
                 */
           AviaryHadoop::HadoopStopResult::HadoopStopResult()
        {

        
                property_Ref  = NULL;
              
            isValidRef  = false;
        
                property_Status  = NULL;
              
            isValidStatus  = false;
        
        }

       AviaryHadoop::HadoopStopResult::HadoopStopResult(AviaryHadoop::HadoopID* arg_Ref,AviaryCommon::Status* arg_Status)
        {
             
               property_Ref  = NULL;
             
            isValidRef  = true;
            
               property_Status  = NULL;
             
            isValidStatus  = true;
            
                    property_Ref = arg_Ref;
            
                    property_Status = arg_Status;
            
        }
        AviaryHadoop::HadoopStopResult::~HadoopStopResult()
        {
            resetAll();
        }

        bool WSF_CALL AviaryHadoop::HadoopStopResult::resetAll()
        {
            //calls reset method for all the properties owned by this method which are pointers.

            
             resetRef();//AviaryHadoop::HadoopID
             resetStatus();//AviaryCommon::Status
            return true;

        }

        

        bool WSF_CALL
        AviaryHadoop::HadoopStopResult::deserialize(axiom_node_t** dp_parent,bool *dp_is_early_node_valid, bool dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          bool status = AXIS2_SUCCESS;
           
         const axis2_char_t* text_value = NULL;
         axutil_qname_t *mqname = NULL;
          
            axutil_qname_t *element_qname = NULL; 
            
               axiom_node_t *first_node = NULL;
               bool is_early_node_valid = true;
               axiom_node_t *current_node = NULL;
               axiom_element_t *current_element = NULL;
            
              
              while(parent && axiom_node_get_node_type(parent, Environment::getEnv()) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, Environment::getEnv());
              }
              if (NULL == parent)
              {   
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, Environment::getEnv());
                      
                    

                     
                     /*
                      * building ref element
                      */
                     
                     
                     
                                   current_node = first_node;
                                   is_early_node_valid = false;
                                   
                                   
                                    while(current_node && axiom_node_get_node_type(current_node, Environment::getEnv()) != AXIOM_ELEMENT)
                                    {
                                        current_node = axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                    }
                                    if(current_node != NULL)
                                    {
                                        current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, Environment::getEnv());
                                        mqname = axiom_element_get_qname(current_element, Environment::getEnv(), current_node);
                                    }
                                   
                                 element_qname = axutil_qname_create(Environment::getEnv(), "ref", NULL, NULL);
                                 

                           if (isParticle() ||  
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("ref", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("ref", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 AviaryHadoop::HadoopID* element = new AviaryHadoop::HadoopID();

                                      status =  element->deserialize(&current_node, &is_early_node_valid, false);
                                      if(AXIS2_FAILURE == status)
                                      {
                                          WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "failed in building adb object for element ref");
                                      }
                                      else
                                      {
                                          status = setRef(element);
                                      }
                                    
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for ref ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, Environment::getEnv());
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building status element
                      */
                     
                     
                     
                                    /*
                                     * because elements are ordered this works fine
                                     */
                                  
                                   
                                   if(current_node != NULL && is_early_node_valid)
                                   {
                                       current_node = axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                       
                                       
                                        while(current_node && axiom_node_get_node_type(current_node, Environment::getEnv()) != AXIOM_ELEMENT)
                                        {
                                            current_node = axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                        }
                                        if(current_node != NULL)
                                        {
                                            current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, Environment::getEnv());
                                            mqname = axiom_element_get_qname(current_element, Environment::getEnv(), current_node);
                                        }
                                       
                                   }
                                   is_early_node_valid = false;
                                 
                                 element_qname = axutil_qname_create(Environment::getEnv(), "status", NULL, NULL);
                                 

                           if (isParticle() ||  
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("status", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("status", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 AviaryCommon::Status* element = new AviaryCommon::Status();

                                      status =  element->deserialize(&current_node, &is_early_node_valid, false);
                                      if(AXIS2_FAILURE == status)
                                      {
                                          WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "failed in building adb object for element status");
                                      }
                                      else
                                      {
                                          status = setStatus(element);
                                      }
                                    
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for status ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, Environment::getEnv());
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                              else if(!dont_care_minoccurs)
                              {
                                  if(element_qname)
                                  {
                                      axutil_qname_free(element_qname, Environment::getEnv());
                                  }
                                  /* this is not a nillable element*/
				  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "non nillable or minOuccrs != 0 element status missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 
          return status;
       }

          bool WSF_CALL
          AviaryHadoop::HadoopStopResult::isParticle()
          {
            
                 return false;
              
          }


          void WSF_CALL
          AviaryHadoop::HadoopStopResult::declareParentNamespaces(
                    axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* WSF_CALL
	AviaryHadoop::HadoopStopResult::serialize(axiom_node_t *parent, 
			axiom_element_t *parent_element, 
			int parent_tag_closed, 
			axutil_hash_t *namespaces, 
			int *next_ns_index)
        {
            
            
             axis2_char_t *string_to_stream;
            
         
         axiom_node_t *current_node = NULL;
         int tag_closed = 0;

         
         
                axiom_namespace_t *ns1 = NULL;

                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                axis2_char_t *p_prefix = NULL;
            
                    axis2_char_t text_value_1[ADB_DEFAULT_DIGIT_LIMIT];
                    
                    axis2_char_t text_value_2[ADB_DEFAULT_DIGIT_LIMIT];
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

            
                    current_node = parent;
                    data_source = (axiom_data_source_t *)axiom_node_get_data_element(current_node, Environment::getEnv());
                    if (!data_source)
                        return NULL;
                    stream = axiom_data_source_get_stream(data_source, Environment::getEnv()); /* assume parent is of type data source */
                    if (!stream)
                        return NULL;
                  
            if(!parent_tag_closed)
            {
            
              string_to_stream = ">"; 
              axutil_stream_write(stream, Environment::getEnv(), string_to_stream, axutil_strlen(string_to_stream));
              tag_closed = 1;
            
            }
            
                       p_prefix = NULL;
                      

                   if (!isValidRef)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("ref"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("ref")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing ref element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sref",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sref>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                     
                            if(!property_Ref->isParticle())
                            {
                                axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                            }
                            property_Ref->serialize(current_node, parent_element,
                                                                                 property_Ref->isParticle() || false, namespaces, next_ns_index);
                            
                            if(!property_Ref->isParticle())
                            {
                                axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                            }
                            
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!isValidStatus)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property status");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("status"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("status")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing status element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sstatus",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sstatus>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                     
                            if(!property_Status->isParticle())
                            {
                                axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                            }
                            property_Status->serialize(current_node, parent_element,
                                                                                 property_Status->isParticle() || false, namespaces, next_ns_index);
                            
                            if(!property_Status->isParticle())
                            {
                                axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                            }
                            
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 

            return parent;
        }


        

            /**
             * Getter for ref by  Property Number 1
             */
            AviaryHadoop::HadoopID* WSF_CALL
            AviaryHadoop::HadoopStopResult::getProperty1()
            {
                return getRef();
            }

            /**
             * getter for ref.
             */
            AviaryHadoop::HadoopID* WSF_CALL
            AviaryHadoop::HadoopStopResult::getRef()
             {
                return property_Ref;
             }

            /**
             * setter for ref
             */
            bool WSF_CALL
            AviaryHadoop::HadoopStopResult::setRef(
                    AviaryHadoop::HadoopID*  arg_Ref)
             {
                

                if(isValidRef &&
                        arg_Ref == property_Ref)
                {
                    
                    return true;
                }

                

                
                resetRef();

                
                    if(NULL == arg_Ref)
                         
                {
                    /* We are already done */
                    return true;
                }
                
                        property_Ref = arg_Ref;
                        isValidRef = true;
                    
                return true;
             }

             

           /**
            * resetter for ref
            */
           bool WSF_CALL
           AviaryHadoop::HadoopStopResult::resetRef()
           {
               int i = 0;
               int count = 0;


               
            
                

                if(property_Ref != NULL)
                {
                   
                   
                         delete  property_Ref;
                     

                   }

                
                
                
               isValidRef = false; 
               return true;
           }

           /**
            * Check whether ref is nill
            */
           bool WSF_CALL
           AviaryHadoop::HadoopStopResult::isRefNil()
           {
               return !isValidRef;
           }

           /**
            * Set ref to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryHadoop::HadoopStopResult::setRefNil()
           {
               return resetRef();
           }

           

            /**
             * Getter for status by  Property Number 2
             */
            AviaryCommon::Status* WSF_CALL
            AviaryHadoop::HadoopStopResult::getProperty2()
            {
                return getStatus();
            }

            /**
             * getter for status.
             */
            AviaryCommon::Status* WSF_CALL
            AviaryHadoop::HadoopStopResult::getStatus()
             {
                return property_Status;
             }

            /**
             * setter for status
             */
            bool WSF_CALL
            AviaryHadoop::HadoopStopResult::setStatus(
                    AviaryCommon::Status*  arg_Status)
             {
                

                if(isValidStatus &&
                        arg_Status == property_Status)
                {
                    
                    return true;
                }

                
                  if(NULL == arg_Status)
                       
                  {
                      WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"status is being set to NULL, but it is not a nullable element");
                      return AXIS2_FAILURE;
                  }
                

                
                resetStatus();

                
                    if(NULL == arg_Status)
                         
                {
                    /* We are already done */
                    return true;
                }
                
                        property_Status = arg_Status;
                        isValidStatus = true;
                    
                return true;
             }

             

           /**
            * resetter for status
            */
           bool WSF_CALL
           AviaryHadoop::HadoopStopResult::resetStatus()
           {
               int i = 0;
               int count = 0;


               
            
                

                if(property_Status != NULL)
                {
                   
                   
                         delete  property_Status;
                     

                   }

                
                
                
               isValidStatus = false; 
               return true;
           }

           /**
            * Check whether status is nill
            */
           bool WSF_CALL
           AviaryHadoop::HadoopStopResult::isStatusNil()
           {
               return !isValidStatus;
           }

           /**
            * Set status to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryHadoop::HadoopStopResult::setStatusNil()
           {
               return resetStatus();
           }

           

