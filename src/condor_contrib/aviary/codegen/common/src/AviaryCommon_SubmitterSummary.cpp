

        /**
         * SubmitterSummary.cpp
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */
        
            #include "AviaryCommon_SubmitterSummary.h"
          

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
        
        using namespace AviaryCommon;
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = SubmitterSummary
                 * Namespace URI = http://common.aviary.grid.redhat.com
                 * Namespace Prefix = ns1
                 */
           AviaryCommon::SubmitterSummary::SubmitterSummary()
        {

        
            isValidRunning  = false;
        
            isValidHeld  = false;
        
            isValidIdle  = false;
        
                property_Created  = NULL;
              
            isValidCreated  = false;
        
                    property_Owner;
                
            isValidOwner  = false;
        
        }

       AviaryCommon::SubmitterSummary::SubmitterSummary(int arg_Running,int arg_Held,int arg_Idle,axutil_date_time_t* arg_Created,std::string arg_Owner)
        {
             
            isValidRunning  = true;
            
            isValidHeld  = true;
            
            isValidIdle  = true;
            
               property_Created  = NULL;
             
            isValidCreated  = true;
            
                 property_Owner;
             
            isValidOwner  = true;
            
                    property_Running = arg_Running;
            
                    property_Held = arg_Held;
            
                    property_Idle = arg_Idle;
            
                    property_Created = arg_Created;
            
                    property_Owner = arg_Owner;
            
        }
        AviaryCommon::SubmitterSummary::~SubmitterSummary()
        {
            resetAll();
        }

        bool WSF_CALL AviaryCommon::SubmitterSummary::resetAll()
        {
            //calls reset method for all the properties owned by this method which are pointers.

            
             resetCreated();//axutil_date_time_t*
            return true;

        }

        

        bool WSF_CALL
        AviaryCommon::SubmitterSummary::deserialize(axiom_node_t** dp_parent,bool *dp_is_early_node_valid, bool dont_care_minoccurs)
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
                      * building running element
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
                                   
                                 element_qname = axutil_qname_create(Environment::getEnv(), "running", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("running", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("running", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, Environment::getEnv(), current_node);
                                      if(text_value != NULL)
                                      {
                                            status = setRunning(atoi(text_value));
                                      }
                                      
                                      else
                                      {
                                          WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "NULL value is set to a non nillable element running");
                                          status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for running ");
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
				  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "non nillable or minOuccrs != 0 element running missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building held element
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
                                 
                                 element_qname = axutil_qname_create(Environment::getEnv(), "held", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("held", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("held", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, Environment::getEnv(), current_node);
                                      if(text_value != NULL)
                                      {
                                            status = setHeld(atoi(text_value));
                                      }
                                      
                                      else
                                      {
                                          WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "NULL value is set to a non nillable element held");
                                          status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for held ");
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
				  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "non nillable or minOuccrs != 0 element held missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building idle element
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
                                 
                                 element_qname = axutil_qname_create(Environment::getEnv(), "idle", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("idle", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("idle", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, Environment::getEnv(), current_node);
                                      if(text_value != NULL)
                                      {
                                            status = setIdle(atoi(text_value));
                                      }
                                      
                                      else
                                      {
                                          WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "NULL value is set to a non nillable element idle");
                                          status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for idle ");
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
				  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "non nillable or minOuccrs != 0 element idle missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building created element
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
                                 
                                 element_qname = axutil_qname_create(Environment::getEnv(), "created", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("created", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("created", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, Environment::getEnv(), current_node);
                                      if(text_value != NULL)
                                      {
                                          axutil_date_time_t* element = axutil_date_time_create(Environment::getEnv());
                                          status = axutil_date_time_deserialize_date_time((axutil_date_time_t*)element, Environment::getEnv(),
                                                                          text_value);
                                          if(AXIS2_FAILURE ==  status)
                                          {
                                              if(element != NULL)
                                              {
                                                  axutil_date_time_free(element, Environment::getEnv());
                                              }
					                          WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI ,"failed in building element created ");
                                          }
                                          else
                                          {
                                            status = setCreated(element);
                                          }
                                      }
                                      
                                      else
                                      {
				                            WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "NULL value is set to a non nillable element created");
                                            status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for created ");
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
				  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "non nillable or minOuccrs != 0 element created missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building owner element
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
                                 
                                 element_qname = axutil_qname_create(Environment::getEnv(), "owner", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("owner", axiom_element_get_localname(current_element, Environment::getEnv())))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("owner", axiom_element_get_localname(current_element, Environment::getEnv()))))
                              {
                                is_early_node_valid = true;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, Environment::getEnv(), current_node);
                                      if(text_value != NULL)
                                      {
                                            status = setOwner(text_value);
                                      }
                                      
                                      else
                                      {
                                            /*
                                             * axis2_qname_t *qname = NULL;
                                             * axiom_attribute_t *the_attri = NULL;
                                             * 
                                             * qname = axutil_qname_create(Environment::getEnv(), "nil", "http://www.w3.org/2001/XMLSchema-instance", "xsi");
                                             * the_attri = axiom_element_get_attribute(current_element, Environment::getEnv(), qname);
                                             */
                                            /* currently thereis a bug in the axiom_element_get_attribute, so we have to go to this bad method */

                                            axiom_attribute_t *the_attri = NULL;
                                            axis2_char_t *attrib_text = NULL;
                                            axutil_hash_t *attribute_hash = NULL;

                                            attribute_hash = axiom_element_get_all_attributes(current_element, Environment::getEnv());

                                            attrib_text = NULL;
                                            if(attribute_hash)
                                            {
                                                 axutil_hash_index_t *hi;
                                                 void *val;
                                                 const void *key;
                                        
                                                 for (hi = axutil_hash_first(attribute_hash, Environment::getEnv()); hi; hi = axutil_hash_next(Environment::getEnv(), hi))
                                                 {
                                                     axutil_hash_this(hi, &key, NULL, &val);
                                                     
                                                     if(strstr((axis2_char_t*)key, "nil|http://www.w3.org/2001/XMLSchema-instance"))
                                                     {
                                                         the_attri = (axiom_attribute_t*)val;
                                                         break;
                                                     }
                                                 }
                                            }

                                            if(the_attri)
                                            {
                                                attrib_text = axiom_attribute_get_value(the_attri, Environment::getEnv());
                                            }
                                            else
                                            {
                                                /* this is hoping that attribute is stored in "http://www.w3.org/2001/XMLSchema-instance", this happnes when name is in default namespace */
                                                attrib_text = axiom_element_get_attribute_value_by_name(current_element, Environment::getEnv(), "nil");
                                            }

                                            if(attrib_text && 0 == axutil_strcmp(attrib_text, "1"))
                                            {
                                                WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "NULL value is set to a non nillable element owner");
                                                status = AXIS2_FAILURE;
                                            }
                                            else
                                            {
                                                /* after all, we found this is a empty string */
                                                status = setOwner("");
                                            }
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"failed in setting the value for owner ");
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
				  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "non nillable or minOuccrs != 0 element owner missing");
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
          AviaryCommon::SubmitterSummary::isParticle()
          {
            
                 return false;
              
          }


          void WSF_CALL
          AviaryCommon::SubmitterSummary::declareParentNamespaces(
                    axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* WSF_CALL
	AviaryCommon::SubmitterSummary::serialize(axiom_node_t *parent, 
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
                    
                    axis2_char_t text_value_3[ADB_DEFAULT_DIGIT_LIMIT];
                    
                    axis2_char_t *text_value_4;
                    axis2_char_t *text_value_4_temp;
                    
                    axis2_char_t *text_value_5;
                    axis2_char_t *text_value_5_temp;
                    
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
                      

                   if (!isValidRunning)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property running");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("running"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("running")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing running element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%srunning>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%srunning>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                               sprintf (text_value_1, AXIS2_PRINTF_INT32_FORMAT_SPECIFIER, property_Running);
                             
                           axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, Environment::getEnv(), text_value_1, axutil_strlen(text_value_1));
                           
                           axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!isValidHeld)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property held");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("held"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("held")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing held element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sheld>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sheld>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                               sprintf (text_value_2, AXIS2_PRINTF_INT32_FORMAT_SPECIFIER, property_Held);
                             
                           axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, Environment::getEnv(), text_value_2, axutil_strlen(text_value_2));
                           
                           axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!isValidIdle)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property idle");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("idle"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("idle")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing idle element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sidle>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sidle>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                               sprintf (text_value_3, AXIS2_PRINTF_INT32_FORMAT_SPECIFIER, property_Idle);
                             
                           axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, Environment::getEnv(), text_value_3, axutil_strlen(text_value_3));
                           
                           axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!isValidCreated)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property created");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("created"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("created")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing created element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%screated>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%screated>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                          text_value_4 = axutil_date_time_serialize_date_time(property_Created, Environment::getEnv());
                           
                           axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, Environment::getEnv(), text_value_4, axutil_strlen(text_value_4));
                           
                           axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!isValidOwner)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property owner");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("owner"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("owner")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing owner element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sowner>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sowner>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                           text_value_5 = (axis2_char_t*)property_Owner.c_str();
                           
                           axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                           
                            
                           text_value_5_temp = axutil_xml_quote_string(Environment::getEnv(), text_value_5, true);
                           if (text_value_5_temp)
                           {
                               axutil_stream_write(stream, Environment::getEnv(), text_value_5_temp, axutil_strlen(text_value_5_temp));
                               AXIS2_FREE(Environment::getEnv()->allocator, text_value_5_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, Environment::getEnv(), text_value_5, axutil_strlen(text_value_5));
                           }
                           
                           axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 

            return parent;
        }


        

            /**
             * Getter for running by  Property Number 1
             */
            int WSF_CALL
            AviaryCommon::SubmitterSummary::getProperty1()
            {
                return getRunning();
            }

            /**
             * getter for running.
             */
            int WSF_CALL
            AviaryCommon::SubmitterSummary::getRunning()
             {
                return property_Running;
             }

            /**
             * setter for running
             */
            bool WSF_CALL
            AviaryCommon::SubmitterSummary::setRunning(
                    const int  arg_Running)
             {
                

                if(isValidRunning &&
                        arg_Running == property_Running)
                {
                    
                    return true;
                }

                

                
                resetRunning();

                
                        property_Running = arg_Running;
                        isValidRunning = true;
                    
                return true;
             }

             

           /**
            * resetter for running
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::resetRunning()
           {
               int i = 0;
               int count = 0;


               
               isValidRunning = false; 
               return true;
           }

           /**
            * Check whether running is nill
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::isRunningNil()
           {
               return !isValidRunning;
           }

           /**
            * Set running to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::setRunningNil()
           {
               return resetRunning();
           }

           

            /**
             * Getter for held by  Property Number 2
             */
            int WSF_CALL
            AviaryCommon::SubmitterSummary::getProperty2()
            {
                return getHeld();
            }

            /**
             * getter for held.
             */
            int WSF_CALL
            AviaryCommon::SubmitterSummary::getHeld()
             {
                return property_Held;
             }

            /**
             * setter for held
             */
            bool WSF_CALL
            AviaryCommon::SubmitterSummary::setHeld(
                    const int  arg_Held)
             {
                

                if(isValidHeld &&
                        arg_Held == property_Held)
                {
                    
                    return true;
                }

                

                
                resetHeld();

                
                        property_Held = arg_Held;
                        isValidHeld = true;
                    
                return true;
             }

             

           /**
            * resetter for held
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::resetHeld()
           {
               int i = 0;
               int count = 0;


               
               isValidHeld = false; 
               return true;
           }

           /**
            * Check whether held is nill
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::isHeldNil()
           {
               return !isValidHeld;
           }

           /**
            * Set held to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::setHeldNil()
           {
               return resetHeld();
           }

           

            /**
             * Getter for idle by  Property Number 3
             */
            int WSF_CALL
            AviaryCommon::SubmitterSummary::getProperty3()
            {
                return getIdle();
            }

            /**
             * getter for idle.
             */
            int WSF_CALL
            AviaryCommon::SubmitterSummary::getIdle()
             {
                return property_Idle;
             }

            /**
             * setter for idle
             */
            bool WSF_CALL
            AviaryCommon::SubmitterSummary::setIdle(
                    const int  arg_Idle)
             {
                

                if(isValidIdle &&
                        arg_Idle == property_Idle)
                {
                    
                    return true;
                }

                

                
                resetIdle();

                
                        property_Idle = arg_Idle;
                        isValidIdle = true;
                    
                return true;
             }

             

           /**
            * resetter for idle
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::resetIdle()
           {
               int i = 0;
               int count = 0;


               
               isValidIdle = false; 
               return true;
           }

           /**
            * Check whether idle is nill
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::isIdleNil()
           {
               return !isValidIdle;
           }

           /**
            * Set idle to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::setIdleNil()
           {
               return resetIdle();
           }

           

            /**
             * Getter for created by  Property Number 4
             */
            axutil_date_time_t* WSF_CALL
            AviaryCommon::SubmitterSummary::getProperty4()
            {
                return getCreated();
            }

            /**
             * getter for created.
             */
            axutil_date_time_t* WSF_CALL
            AviaryCommon::SubmitterSummary::getCreated()
             {
                return property_Created;
             }

            /**
             * setter for created
             */
            bool WSF_CALL
            AviaryCommon::SubmitterSummary::setCreated(
                    axutil_date_time_t*  arg_Created)
             {
                

                if(isValidCreated &&
                        arg_Created == property_Created)
                {
                    
                    return true;
                }

                
                  if(NULL == arg_Created)
                       
                  {
                      WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"created is being set to NULL, but it is not a nullable element");
                      return AXIS2_FAILURE;
                  }
                

                
                resetCreated();

                
                    if(NULL == arg_Created)
                         
                {
                    /* We are already done */
                    return true;
                }
                
                        property_Created = arg_Created;
                        isValidCreated = true;
                    
                return true;
             }

             

           /**
            * resetter for created
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::resetCreated()
           {
               int i = 0;
               int count = 0;


               
            
                

                if(property_Created != NULL)
                {
                   
                   
                      axutil_date_time_free(property_Created, Environment::getEnv());
                         property_Created = NULL;
                     

                   }

                
                
                
               isValidCreated = false; 
               return true;
           }

           /**
            * Check whether created is nill
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::isCreatedNil()
           {
               return !isValidCreated;
           }

           /**
            * Set created to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::setCreatedNil()
           {
               return resetCreated();
           }

           

            /**
             * Getter for owner by  Property Number 5
             */
            std::string WSF_CALL
            AviaryCommon::SubmitterSummary::getProperty5()
            {
                return getOwner();
            }

            /**
             * getter for owner.
             */
            std::string WSF_CALL
            AviaryCommon::SubmitterSummary::getOwner()
             {
                return property_Owner;
             }

            /**
             * setter for owner
             */
            bool WSF_CALL
            AviaryCommon::SubmitterSummary::setOwner(
                    const std::string  arg_Owner)
             {
                

                if(isValidOwner &&
                        arg_Owner == property_Owner)
                {
                    
                    return true;
                }

                
                  if(arg_Owner.empty())
                       
                  {
                      WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"owner is being set to NULL, but it is not a nullable element");
                      return AXIS2_FAILURE;
                  }
                

                
                resetOwner();

                
                        property_Owner = std::string(arg_Owner.c_str());
                        isValidOwner = true;
                    
                return true;
             }

             

           /**
            * resetter for owner
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::resetOwner()
           {
               int i = 0;
               int count = 0;


               
               isValidOwner = false; 
               return true;
           }

           /**
            * Check whether owner is nill
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::isOwnerNil()
           {
               return !isValidOwner;
           }

           /**
            * Set owner to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCommon::SubmitterSummary::setOwnerNil()
           {
               return resetOwner();
           }

           

