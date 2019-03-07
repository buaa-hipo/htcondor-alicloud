

        /**
         * GetAttributes.cpp
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */
        
            #include "AviaryCollector_GetAttributes.h"
          

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
        
        using namespace AviaryCollector;
        
               /*
                * Implementation of the GetAttributes|http://collector.aviary.grid.redhat.com Element
                */
           AviaryCollector::GetAttributes::GetAttributes()
        {

        
            qname = NULL;
        
                property_Ids  = NULL;
              
            isValidIds  = false;
        
            isValidValuesOnly  = false;
        
                  qname =  axutil_qname_create (Environment::getEnv(),
                        "GetAttributes",
                        "http://collector.aviary.grid.redhat.com",
                        NULL);
                
        }

       AviaryCollector::GetAttributes::GetAttributes(std::vector<AviaryCollector::AttributeRequest*>* arg_Ids,bool arg_ValuesOnly)
        {
             
                   qname = NULL;
             
               property_Ids  = NULL;
             
            isValidIds  = true;
            
            isValidValuesOnly  = true;
            
                 qname =  axutil_qname_create (Environment::getEnv(),
                       "GetAttributes",
                       "http://collector.aviary.grid.redhat.com",
                       NULL);
               
                    property_Ids = arg_Ids;
            
                    property_ValuesOnly = arg_ValuesOnly;
            
        }
        AviaryCollector::GetAttributes::~GetAttributes()
        {
            resetAll();
        }

        bool WSF_CALL AviaryCollector::GetAttributes::resetAll()
        {
            //calls reset method for all the properties owned by this method which are pointers.

            
             resetIds();//AviaryCollector::AttributeRequest
          if(qname != NULL)
          {
            axutil_qname_free( qname, Environment::getEnv());
            qname = NULL;
          }
        
            return true;

        }

        

        bool WSF_CALL
        AviaryCollector::GetAttributes::deserialize(axiom_node_t** dp_parent,bool *dp_is_early_node_valid, bool dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          bool status = AXIS2_SUCCESS;
          
          axiom_attribute_t *parent_attri = NULL;
          axiom_element_t *parent_element = NULL;
          axis2_char_t *attrib_text = NULL;

          axutil_hash_t *attribute_hash = NULL;

           
         const axis2_char_t* text_value = NULL;
         axutil_qname_t *mqname = NULL;
          
               int i = 0;
            
               int sequence_broken = 0;
               axiom_node_t *tmp_node = NULL;
            
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
              

                    current_element = (axiom_element_t *)axiom_node_get_data_element(parent, Environment::getEnv());
                    mqname = axiom_element_get_qname(current_element, Environment::getEnv(), parent);
                    if (axutil_qname_equals(mqname, Environment::getEnv(), this->qname))
                    {
                        
                          first_node = axiom_node_get_first_child(parent, Environment::getEnv());
                          
                    }
                    else
                    {
                        WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI,
                              "Failed in building adb object for GetAttributes : "
                              "Expected %s but returned %s",
                              axutil_qname_to_string(qname, Environment::getEnv()),
                              axutil_qname_to_string(mqname, Environment::getEnv()));
                        
                        return AXIS2_FAILURE;
                    }
                    
                 parent_element = (axiom_element_t *)axiom_node_get_data_element(parent, Environment::getEnv());
                 attribute_hash = axiom_element_get_all_attributes(parent_element, Environment::getEnv());
              
                       { 
                    /*
                     * building Ids array
                     */
                       std::vector<AviaryCollector::AttributeRequest*>* arr_list =new std::vector<AviaryCollector::AttributeRequest*>();
                   

                     
                     /*
                      * building ids element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(Environment::getEnv(), "ids", NULL, NULL);
                                  
                               
                               for (i = 0, sequence_broken = 0, current_node = first_node; !sequence_broken && current_node != NULL;)
                                             
                               {
                                  if(axiom_node_get_node_type(current_node, Environment::getEnv()) != AXIOM_ELEMENT)
                                  {
                                     current_node =axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                     is_early_node_valid = false;
                                     continue;
                                  }
                                  
                                  current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, Environment::getEnv());
                                  mqname = axiom_element_get_qname(current_element, Environment::getEnv(), current_node);

                                  if (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("ids", axiom_element_get_localname(current_element, Environment::getEnv())))
                                  {
                                  
                                      is_early_node_valid = true;
                                      
                                     AviaryCollector::AttributeRequest* element = new AviaryCollector::AttributeRequest();
                                          
                                          status =  element->deserialize(&current_node, &is_early_node_valid, false);
                                          
                                          if(AXIS2_FAILURE ==  status)
                                          {
					  WSF_LOG_ERROR_MSG(Environment::getEnv()->log,WSF_LOG_SI, "failed in building element ids ");
                                          }
                                          else
                                          {
                                            arr_list->push_back(element);
                                            
                                          }
                                        
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "failed in setting the value for ids ");
                                         if(element_qname)
                                         {
                                            axutil_qname_free(element_qname, Environment::getEnv());
                                         }
                                         if(arr_list)
                                         {
                                            delete arr_list;
                                         }
                                         return false;
                                     }

                                     i++;
                                    current_node = axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                  }
                                  else
                                  {
                                      is_early_node_valid = false;
                                      sequence_broken = 1;
                                  }
                                  
                               }

                               
                                   if (i < 1)
                                   {
                                     /* found element out of order */
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"ids (@minOccurs = '1') only have %d elements", i);
                                     if(element_qname)
                                     {
                                        axutil_qname_free(element_qname, Environment::getEnv());
                                     }
                                     if(arr_list)
                                     {
                                        delete arr_list;
                                     }
                                     return false;
                                   }
                               

                               if(0 == arr_list->size())
                               {
                                    delete arr_list;
                               }
                               else
                               {
                                    status = setIds(arr_list);
                               }

                              
                            } 
                        
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 
                
                
                  parent_attri = NULL;
                  attrib_text = NULL;
                  if(attribute_hash)
                  {
                       axutil_hash_index_t *hi;
                       void *val;
                       const void *key;

                       for (hi = axutil_hash_first(attribute_hash, Environment::getEnv()); hi; hi = axutil_hash_next(Environment::getEnv(), hi))
                       {
                           axutil_hash_this(hi, &key, NULL, &val);
                           
                           
                               if(!strcmp((axis2_char_t*)key, "valuesOnly"))
                             
                               {
                                   parent_attri = (axiom_attribute_t*)val;
                                   break;
                               }
                       }
                  }

                  if(parent_attri)
                  {
                    attrib_text = axiom_attribute_get_value(parent_attri, Environment::getEnv());
                  }
                  else
                  {
                    /* this is hoping that attribute is stored in "valuesOnly", this happnes when name is in default namespace */
                    attrib_text = axiom_element_get_attribute_value_by_name(parent_element, Environment::getEnv(), "valuesOnly");
                  }

                  if(attrib_text != NULL)
                  {
                      
                      
                           if (!axutil_strcmp(attrib_text, "TRUE") || !axutil_strcmp(attrib_text, "true"))
                           {
                               setValuesOnly(true);
                           }
                           else
                           {
                               setValuesOnly(false);
                           }
                        
                    }
                  
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 
          return status;
       }

          bool WSF_CALL
          AviaryCollector::GetAttributes::isParticle()
          {
            
                 return false;
              
          }


          void WSF_CALL
          AviaryCollector::GetAttributes::declareParentNamespaces(
                    axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* WSF_CALL
	AviaryCollector::GetAttributes::serialize(axiom_node_t *parent, 
			axiom_element_t *parent_element, 
			int parent_tag_closed, 
			axutil_hash_t *namespaces, 
			int *next_ns_index)
        {
            
            
               axiom_attribute_t *text_attri = NULL;
             
             axis2_char_t *string_to_stream;
            
         
         axiom_node_t *current_node = NULL;
         int tag_closed = 0;

         
         
                axiom_namespace_t *ns1 = NULL;

                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                axis2_char_t *p_prefix = NULL;
            
               int i = 0;
               int count = 0;
               void *element = NULL;
             
                    axis2_char_t text_value_1[ADB_DEFAULT_DIGIT_LIMIT];
                    
                    axis2_char_t text_value_2[ADB_DEFAULT_DIGIT_LIMIT];
                    
                axis2_char_t *text_value = NULL;
             
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

             
                int next_ns_index_value = 0;
             
                    namespaces = axutil_hash_make(Environment::getEnv());
                    next_ns_index = &next_ns_index_value;
                     
                           ns1 = axiom_namespace_create (Environment::getEnv(),
                                             "http://collector.aviary.grid.redhat.com",
                                             "n"); 
                           axutil_hash_set(namespaces, "http://collector.aviary.grid.redhat.com", AXIS2_HASH_KEY_STRING, axutil_strdup(Environment::getEnv(), "n"));
                       
                     
                    parent_element = axiom_element_create (Environment::getEnv(), NULL, "GetAttributes", ns1 , &parent);
                    
                    
                    axiom_element_set_namespace(parent_element, Environment::getEnv(), ns1, parent);


            
                    data_source = axiom_data_source_create(Environment::getEnv(), parent, &current_node);
                    stream = axiom_data_source_get_stream(data_source, Environment::getEnv());
                  
            if(!parent_tag_closed)
            {
            
                if(isValidValuesOnly)
                {
                
                        p_prefix = NULL;
                      
                           
                           text_value = (axis2_char_t*)((property_ValuesOnly)?"true":"false");
                           string_to_stream = (axis2_char_t*) AXIS2_MALLOC (Environment::getEnv()-> allocator, sizeof (axis2_char_t) *
                                                            (5  + ADB_DEFAULT_NAMESPACE_PREFIX_LIMIT +
                                                             axutil_strlen(text_value) + 
                                                             axutil_strlen("valuesOnly")));
                           sprintf(string_to_stream, " %s%s%s=\"%s\"", p_prefix?p_prefix:"", (p_prefix && axutil_strcmp(p_prefix, ""))?":":"",
                                                "valuesOnly",  text_value);
                           axutil_stream_write(stream, Environment::getEnv(), string_to_stream, axutil_strlen(string_to_stream));
                           AXIS2_FREE(Environment::getEnv()-> allocator, string_to_stream);
                        
                   }
                   
            }
            
                       p_prefix = NULL;
                      

                   if (!isValidIds)
                   {
                      
                            
                            WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"Nil value found in non-nillable property ids");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("ids"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("ids")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing Ids array
                      */
                     if (property_Ids != NULL)
                     {
                        

                            sprintf(start_input_str, "<%s%sids",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%sids>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = property_Ids->size();
                         for(i = 0; i < count; i++)
                         {
                            AviaryCollector::AttributeRequest* element = (*property_Ids)[i];

                            if(NULL == element) 
                            {
                                continue;
                            }

                    
                     
                     /*
                      * parsing ids element
                      */

                    
                     
                            if(!element->isParticle())
                            {
                                axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                            }
                            element->serialize(current_node, parent_element,
                                                                                 element->isParticle() || false, namespaces, next_ns_index);
                            
                            if(!element->isParticle())
                            {
                                axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                            }
                            
                         }
                     }
                   
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                    
                    if(parent_tag_closed)
                    {
                       if(isValidValuesOnly)
                       {
                       
                           p_prefix = NULL;
                           ns1 = NULL;
                         
                           
                           text_value =  (axis2_char_t*)((property_ValuesOnly)?axutil_strdup(Environment::getEnv(), "true"):axutil_strdup(Environment::getEnv(), "false"));
                           text_attri = axiom_attribute_create (Environment::getEnv(), "valuesOnly", text_value, ns1);
                           axiom_element_add_attribute (parent_element, Environment::getEnv(), text_attri, parent);
                           AXIS2_FREE(Environment::getEnv()->allocator, text_value);
                        
                      }
                       
                  }
                
                   if(namespaces)
                   {
                       axutil_hash_index_t *hi;
                       void *val;
                       for (hi = axutil_hash_first(namespaces, Environment::getEnv()); hi; hi = axutil_hash_next(Environment::getEnv(), hi))
                       {
                           axutil_hash_this(hi, NULL, NULL, &val);
                           AXIS2_FREE(Environment::getEnv()->allocator, val);
                       }
                       axutil_hash_free(namespaces, Environment::getEnv());
                   }
                

            return parent;
        }


        

            /**
             * Getter for ids by  Property Number 1
             */
            std::vector<AviaryCollector::AttributeRequest*>* WSF_CALL
            AviaryCollector::GetAttributes::getProperty1()
            {
                return getIds();
            }

            /**
             * getter for ids.
             */
            std::vector<AviaryCollector::AttributeRequest*>* WSF_CALL
            AviaryCollector::GetAttributes::getIds()
             {
                return property_Ids;
             }

            /**
             * setter for ids
             */
            bool WSF_CALL
            AviaryCollector::GetAttributes::setIds(
                    std::vector<AviaryCollector::AttributeRequest*>*  arg_Ids)
             {
                
                 int size = 0;
                 int i = 0;
                 bool non_nil_exists = false;
                

                if(isValidIds &&
                        arg_Ids == property_Ids)
                {
                    
                    return true;
                }

                
                 size = arg_Ids->size();
                 
                 if (size < 1)
                 {
                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"ids has less than minOccurs(1)");
                     return false;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != (*arg_Ids)[i])
                     {
                         non_nil_exists = true;
                         break;
                     }
                 }

                 
                    if(!non_nil_exists)
                    {
                        WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"All the elements in the array of ids is being set to NULL, but it is not a nullable or minOccurs=0 element");
                        return false;
                    }
                 
                  if(NULL == arg_Ids)
                       
                  {
                      WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"ids is being set to NULL, but it is not a nullable element");
                      return AXIS2_FAILURE;
                  }
                

                
                resetIds();

                
                    if(NULL == arg_Ids)
                         
                {
                    /* We are already done */
                    return true;
                }
                
                        property_Ids = arg_Ids;
                        if(non_nil_exists)
                        {
                            isValidIds = true;
                        }
                        
                    
                return true;
             }

            
            /**
             * Get ith element of ids.
             */
            AviaryCollector::AttributeRequest* WSF_CALL
            AviaryCollector::GetAttributes::getIdsAt(int i)
            {
                AviaryCollector::AttributeRequest* ret_val;
                if(property_Ids == NULL)
                {
                    return (AviaryCollector::AttributeRequest*)0;
                }
                ret_val =   (*property_Ids)[i];
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of ids.
             */
           bool WSF_CALL
            AviaryCollector::GetAttributes::setIdsAt(int i,
                    AviaryCollector::AttributeRequest* arg_Ids)
            {
                 AviaryCollector::AttributeRequest* element;
                int size = 0;

                int non_nil_count;
                bool non_nil_exists = false;

                 

                if( isValidIds &&
                    property_Ids &&
                  
                    arg_Ids == (*property_Ids)[i])
                  
                 {
                    
                    return AXIS2_SUCCESS; 
                }

                   
                     non_nil_exists = true;
                  
                   if(!non_nil_exists)
                   {
                       WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "All the elements in the array of ids is being set to NULL, but it is not a nullable or minOccurs=0 element");
                       return AXIS2_FAILURE;
                   }
                

                if(property_Ids == NULL)
                {
                    property_Ids = new std::vector<AviaryCollector::AttributeRequest*>();
                }
                else{
                /* check whether there already exist an element */
                element = (*property_Ids)[i];
                }

                
                        if(NULL != element)
                        {
                          
                          
                          
                                delete element;
                             
                        }
                        
                    
                    (*property_Ids)[i] = arg_Ids;
                  

               isValidIds = true;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to ids.
             */
            bool WSF_CALL
            AviaryCollector::GetAttributes::addIds(
                    AviaryCollector::AttributeRequest* arg_Ids)
             {

                
                    if( NULL == arg_Ids
                     )
                    {
                      
                           WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "All the elements in the array of ids is being set to NULL, but it is not a nullable or minOccurs=0 element");
                           return false;
                        
                    }
                  

                if(property_Ids == NULL)
                {
                    property_Ids = new std::vector<AviaryCollector::AttributeRequest*>();
                }
              
               property_Ids->push_back(arg_Ids);
              
                isValidIds = true;
                return true;
             }

            /**
             * Get the size of the ids array.
             */
            int WSF_CALL
            AviaryCollector::GetAttributes::sizeofIds()
            {

                if(property_Ids == NULL)
                {
                    return 0;
                }
                return property_Ids->size();
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            bool WSF_CALL
            AviaryCollector::GetAttributes::removeIdsAt(int i)
            {
                return setIdsNilAt(i);
            }

            

           /**
            * resetter for ids
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::resetIds()
           {
               int i = 0;
               int count = 0;


               
                if (property_Ids != NULL)
                {
                  std::vector<AviaryCollector::AttributeRequest*>::iterator it =  property_Ids->begin();
                  for( ; it <  property_Ids->end() ; ++it)
                  {
                     AviaryCollector::AttributeRequest* element = *it;
                
            
                

                if(element != NULL)
                {
                   
                   
                         delete  element;
                     

                   }

                
                
                
               }

             }
                
                    if(NULL != property_Ids)
                 delete property_Ids;
                
               isValidIds = false; 
               return true;
           }

           /**
            * Check whether ids is nill
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::isIdsNil()
           {
               return !isValidIds;
           }

           /**
            * Set ids to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::setIdsNil()
           {
               return resetIds();
           }

           
           /**
            * Check whether ids is nill at i
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::isIdsNilAt(int i)
           {
               return (isValidIds == false ||
                       NULL == property_Ids ||
                     NULL == (*property_Ids)[i]);
            }

           /**
            * Set ids to nil at i
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::setIdsNilAt(int i)
           {
                int size = 0;
                int j;
                bool non_nil_exists = false;

                int k = 0;

                if(property_Ids == NULL ||
                            isValidIds == false)
                {
                    
                    non_nil_exists = false;
                }
                else
                {
                    size = property_Ids->size();
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != (*property_Ids)[i])
                        {
                            k++;
                            non_nil_exists = true;
                            if( k >= 1)
                            {
                                break;
                            }
                        }
                    }
                }
                
                   if(!non_nil_exists)
                   {
                       WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "All the elements in the array of ids is being set to NULL, but it is not a nullable or minOccurs=0 element");
                       return AXIS2_FAILURE;
                   }
                

                if( k < 1)
                {
                       WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "Size of the array of ids is beinng set to be smaller than the specificed number of minOccurs(1)");
                       return AXIS2_FAILURE;
                }
 
                if(property_Ids == NULL)
                {
                    isValidIds = false;
                    
                    return true;
                }
                 
                 /* check whether there already exist an element */
                 AviaryCollector::AttributeRequest* element = (*property_Ids)[i];
                if(NULL != element)
                {
                  
                  
                  
                        delete element;
                     
                 }
                 

                
                (*property_Ids)[i] = NULL;
                
                return AXIS2_SUCCESS;

           }

           

            /**
             * Getter for valuesOnly by  Property Number 2
             */
            bool WSF_CALL
            AviaryCollector::GetAttributes::getProperty2()
            {
                return getValuesOnly();
            }

            /**
             * getter for valuesOnly.
             */
            bool WSF_CALL
            AviaryCollector::GetAttributes::getValuesOnly()
             {
                return property_ValuesOnly;
             }

            /**
             * setter for valuesOnly
             */
            bool WSF_CALL
            AviaryCollector::GetAttributes::setValuesOnly(
                    bool  arg_ValuesOnly)
             {
                

                if(isValidValuesOnly &&
                        arg_ValuesOnly == property_ValuesOnly)
                {
                    
                    return true;
                }

                

                
                resetValuesOnly();

                
                        property_ValuesOnly = arg_ValuesOnly;
                        isValidValuesOnly = true;
                    
                return true;
             }

             

           /**
            * resetter for valuesOnly
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::resetValuesOnly()
           {
               int i = 0;
               int count = 0;


               
               isValidValuesOnly = false; 
               return true;
           }

           /**
            * Check whether valuesOnly is nill
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::isValuesOnlyNil()
           {
               return !isValidValuesOnly;
           }

           /**
            * Set valuesOnly to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCollector::GetAttributes::setValuesOnlyNil()
           {
               return resetValuesOnly();
           }

           

