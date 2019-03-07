
          #ifndef AviaryCollector_GETATTRIBUTES_H
          #define AviaryCollector_GETATTRIBUTES_H
        
      
       /**
        * GetAttributes.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.0  Built on : Nov 08, 2012 (09:07:42 EST)
        */

       /**
        *  GetAttributes class
        */

        namespace AviaryCollector{
            class GetAttributes;
        }
        

        
                #include "AviaryCollector_AttributeRequest.h"
              
        #include <axutil_qname.h>
        

        #include <stdio.h>
        #include <OMElement.h>
        #include <ServiceClient.h>
        #include <ADBDefines.h>

namespace AviaryCollector
{
        
        

        class GetAttributes {

        private:
             
                axutil_qname_t* qname;
            std::vector<AviaryCollector::AttributeRequest*>* property_Ids;

                
                bool isValidIds;
            bool property_ValuesOnly;

                
                bool isValidValuesOnly;
            

        /*** Private methods ***/
          

        bool WSF_CALL
        setIdsNil();
            



        /******************************* public functions *********************************/

        public:

        /**
         * Constructor for class GetAttributes
         */

        GetAttributes();

        /**
         * Destructor GetAttributes
         */
        ~GetAttributes();


       

        /**
         * Constructor for creating GetAttributes
         * @param 
         * @param Ids std::vector<AviaryCollector::AttributeRequest*>*
         * @param ValuesOnly bool
         * @return newly created GetAttributes object
         */
        GetAttributes(std::vector<AviaryCollector::AttributeRequest*>* arg_Ids,bool arg_ValuesOnly);
        

        /**
         * resetAll for GetAttributes
         */
        WSF_EXTERN bool WSF_CALL resetAll();
        
        /********************************** Class get set methods **************************************/
        /******** Deprecated for array types, Use 'Getters and Setters for Arrays' instead ***********/
        

        /**
         * Getter for ids. Deprecated for array types, Use getIdsAt instead
         * @return Array of AviaryCollector::AttributeRequest*s.
         */
        WSF_EXTERN std::vector<AviaryCollector::AttributeRequest*>* WSF_CALL
        getIds();

        /**
         * Setter for ids.Deprecated for array types, Use setIdsAt
         * or addIds instead.
         * @param arg_Ids Array of AviaryCollector::AttributeRequest*s.
         * @return true on success, false otherwise
         */
        WSF_EXTERN bool WSF_CALL
        setIds(std::vector<AviaryCollector::AttributeRequest*>*  arg_Ids);

        /**
         * Re setter for ids
         * @return true on success, false
         */
        WSF_EXTERN bool WSF_CALL
        resetIds();
        
        

        /**
         * Getter for valuesOnly. 
         * @return bool
         */
        WSF_EXTERN bool WSF_CALL
        getValuesOnly();

        /**
         * Setter for valuesOnly.
         * @param arg_ValuesOnly bool
         * @return true on success, false otherwise
         */
        WSF_EXTERN bool WSF_CALL
        setValuesOnly(bool  arg_ValuesOnly);

        /**
         * Re setter for valuesOnly
         * @return true on success, false
         */
        WSF_EXTERN bool WSF_CALL
        resetValuesOnly();
        
        /****************************** Get Set methods for Arrays **********************************/
        /************ Array Specific Operations: get_at, set_at, add, remove_at, sizeof *****************/

        /**
         * E.g. use of get_at, set_at, add and sizeof
         *
         * for(i = 0; i < adb_element->sizeofProperty(); i ++ )
         * {
         *     // Getting ith value to property_object variable
         *     property_object = adb_element->getPropertyAt(i);
         *
         *     // Setting ith value from property_object variable
         *     adb_element->setPropertyAt(i, property_object);
         *
         *     // Appending the value to the end of the array from property_object variable
         *     adb_element->addProperty(property_object);
         *
         *     // Removing the ith value from an array
         *     adb_element->removePropertyAt(i);
         *     
         * }
         *
         */

        
        
        /**
         * Get the ith element of ids.
        * @param i index of the item to be obtained
         * @return ith AviaryCollector::AttributeRequest* of the array
         */
        WSF_EXTERN AviaryCollector::AttributeRequest* WSF_CALL
        getIdsAt(int i);

        /**
         * Set the ith element of ids. (If the ith already exist, it will be replaced)
         * @param i index of the item to return
         * @param arg_Ids element to set AviaryCollector::AttributeRequest* to the array
         * @return ith AviaryCollector::AttributeRequest* of the array
         */
        WSF_EXTERN bool WSF_CALL
        setIdsAt(int i,
                AviaryCollector::AttributeRequest* arg_Ids);


        /**
         * Add to ids.
         * @param arg_Ids element to add AviaryCollector::AttributeRequest* to the array
         * @return true on success, false otherwise.
         */
        WSF_EXTERN bool WSF_CALL
        addIds(
            AviaryCollector::AttributeRequest* arg_Ids);

        /**
         * Get the size of the ids array.
         * @return the size of the ids array.
         */
        WSF_EXTERN int WSF_CALL
        sizeofIds();

        /**
         * Remove the ith element of ids.
         * @param i index of the item to remove
         * @return true on success, false otherwise.
         */
        WSF_EXTERN bool WSF_CALL
        removeIdsAt(int i);

        


        /******************************* Checking and Setting NIL values *********************************/
        /* Use 'Checking and Setting NIL values for Arrays' to check and set nil for individual elements */

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether ids is Nill
         * @return true if the element is Nil, false otherwise
         */
        bool WSF_CALL
        isIdsNil();


        

        /**
         * Check whether valuesOnly is Nill
         * @return true if the element is Nil, false otherwise
         */
        bool WSF_CALL
        isValuesOnlyNil();


        
        /**
         * Set valuesOnly to Nill (same as using reset)
         * @return true on success, false otherwise.
         */
        bool WSF_CALL
        setValuesOnlyNil();
        

        /*************************** Checking and Setting 'NIL' values in Arrays *****************************/

        /**
         * NOTE: You may set this to remove specific elements in the array
         *       But you can not remove elements, if the specific property is declared to be non-nillable or sizeof(array) < minOccurs
         */
        
        /**
         * Check whether ids is Nill at position i
         * @param i index of the item to return.
         * @return true if the value is Nil at position i, false otherwise
         */
        bool WSF_CALL
        isIdsNilAt(int i);
 
       
        /**
         * Set ids to NILL at the  position i.
         * @param i . The index of the item to be set Nill.
         * @return true on success, false otherwise.
         */
        bool WSF_CALL
        setIdsNilAt(int i);

        

        /**************************** Serialize and De serialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize the ADB object to an XML
         * @param dp_parent double pointer to the parent node to be deserialized
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return true on success, false otherwise
         */
        bool WSF_CALL
        deserialize(axiom_node_t** omNode, bool *isEarlyNodeValid, bool dontCareMinoccurs);
                         
            

       /**
         * Declare namespace in the most parent node 
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
        void WSF_CALL
        declareParentNamespaces(axiom_element_t *parent_element, axutil_hash_t *namespaces, int *next_ns_index);


        

        /**
         * Serialize the ADB object to an xml
         * @param GetAttributes_om_node node to serialize from
         * @param GetAttributes_om_element parent element to serialize from
         * @param tag_closed Whether the parent tag is closed or not
         * @param namespaces hash of namespace uris to prefixes
         * @param next_ns_index an int which contains the next namespace index
         * @return axiom_node_t on success,NULL otherwise.
         */
        axiom_node_t* WSF_CALL
        serialize(axiom_node_t* GetAttributes_om_node, axiom_element_t *GetAttributes_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the GetAttributes is a particle class (E.g. group, inner sequence)
         * @return true if this is a particle class, false otherwise.
         */
        bool WSF_CALL
        isParticle();



        /******************************* get the value by the property number  *********************************/
        /************NOTE: This method is introduced to resolve a problem in unwrapping mode *******************/

      
        

        /**
         * Getter for ids by property number (1)
         * @return Array of AviaryCollector::AttributeRequests.
         */

        std::vector<AviaryCollector::AttributeRequest*>* WSF_CALL
        getProperty1();

    
        

        /**
         * Getter for valuesOnly by property number (2)
         * @return bool
         */

        bool WSF_CALL
        getProperty2();

    

};

}        
 #endif /* GETATTRIBUTES_H */
    

