/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/



/*****
  NOTE : this file is just #included, so it should not include
  condor_common.h !
*****/

static bool
convert_ad_to_adStruct(struct soap *s,
                       ClassAd *curr_ad,
                       struct condor__ClassAdStruct *ad_struct,
					   bool isDeepCopy)
{
  int attr_index = 0;
  int num_attrs = 0;
  bool skip_attr = false;
  int tmpint;
  float tmpfloat;
  bool tmpbool;
  char *tmpstr;
  ExprTree *tree;
  const char *name;

  if ( !ad_struct ) {
    return false;
  }

  if ( !curr_ad ) {
    // send back an empty array
    ad_struct->__size = 0;
    ad_struct->__ptr = NULL;
    return true;
  }

  // first pass: count attrs
  num_attrs = 0;
  curr_ad->ResetExpr();
  while( curr_ad->NextExpr(name, tree) ) {
    if( name && tree ) {
      num_attrs++;
    }
  }

  if ( num_attrs == 0 ) {
    // send back an empty array
    ad_struct->__size = 0;
    ad_struct->__ptr = NULL;
    return true;
  }

  // We have to add MyType and TargetType manually.
  // Argh, and ServerTime. XXX: This is getting silly, we need a schema.
  num_attrs += 2;
  num_attrs += 1;

  // allocate space for attributes
  ad_struct->__size = num_attrs;
  ad_struct->__ptr = (struct condor__ClassAdStructAttr *)
    soap_malloc(s,num_attrs * sizeof(struct condor__ClassAdStructAttr));

  // second pass: serialize attributes
  attr_index = 0;
  // first, add myType and TargetType
  ad_struct->__ptr[attr_index].name = (char *) ATTR_MY_TYPE;
  ad_struct->__ptr[attr_index].type = STRING_ATTR;
  if (isDeepCopy) {
	  ad_struct->__ptr[attr_index].value =
		  (char *) soap_malloc(s, strlen(GetMyTypeName(*curr_ad)) + 1);
	  strcpy(ad_struct->__ptr[attr_index].value, GetMyTypeName(*curr_ad));
  } else {
	  ad_struct->__ptr[attr_index].value = (char *) GetMyTypeName(*curr_ad);
  }
  attr_index++;
  ad_struct->__ptr[attr_index].name = (char *) ATTR_TARGET_TYPE;
  ad_struct->__ptr[attr_index].type = STRING_ATTR;
  if (isDeepCopy) {
	  ad_struct->__ptr[attr_index].value =
		  (char *) soap_malloc(s, strlen(GetTargetTypeName(*curr_ad)) + 1);
	  strcpy(ad_struct->__ptr[attr_index].value, GetTargetTypeName(*curr_ad));
  } else {
	  ad_struct->__ptr[attr_index].value =
		  (char *) GetTargetTypeName(*curr_ad);
  }
  attr_index++;
  // And, ServerTime...
  ad_struct->__ptr[attr_index].name = (char *) ATTR_SERVER_TIME;
  ad_struct->__ptr[attr_index].type = INTEGER_ATTR;
  MyString timeString = MyString((int) time(NULL));
  ad_struct->__ptr[attr_index].value =
	  (char *) soap_malloc(s, strlen(timeString.Value()) + 1);
  strcpy(ad_struct->__ptr[attr_index].value, timeString.Value());
  attr_index++;

  curr_ad->ResetExpr();
  while( curr_ad->NextExpr(name, tree) ) {
    // ad_struct->__ptr[attr_index].valueInt = NULL;
    // ad_struct->__ptr[attr_index].valueFloat = NULL;
    // ad_struct->__ptr[attr_index].valueBool = NULL;
    // ad_struct->__ptr[attr_index].valueExpr = NULL;

	// We want to ignore old ServerTime attributes that might be in the
	// ad. What we really really want is to not have to add any special
	// attribute at all, but that will not happen until we are using
	// new ClassAds.
	if (0 == strcmp(name, ATTR_MY_TYPE) ||
		0 == strcmp(name, ATTR_TARGET_TYPE) ||
		0 == strcmp(name, ATTR_SERVER_TIME)) 
	{
		continue;
	}

	// Ignore any attributes that are considered private - we don't wanna 
	// be handing out private attributes to soap clients.
	if ( ClassAdAttributeIsPrivate(name) )
	{
		continue;
	}

    skip_attr = false;
	if ( tree->GetKind() == classad::ExprTree::LITERAL_NODE ) {
		classad::Value val;
		((classad::Literal *)tree)->GetValue( val );
		switch ( val.GetType() ) {
		case classad::Value::STRING_VALUE:
			const char *str;
			val.IsStringValue( str );
			ad_struct->__ptr[attr_index].value =
				(char *) soap_malloc(s, strlen(str) + 1);
			strcpy(ad_struct->__ptr[attr_index].value,str);
			ad_struct->__ptr[attr_index].type = STRING_ATTR;
			break;
		case classad::Value::INTEGER_VALUE:
			val.IsIntegerValue( tmpint );
			ad_struct->__ptr[attr_index].value = (char*)soap_malloc(s,20);
			snprintf(ad_struct->__ptr[attr_index].value,20,"%d",tmpint);
			ad_struct->__ptr[attr_index].type = INTEGER_ATTR;
			break;
		case classad::Value::REAL_VALUE:
			double tmpdouble;
			val.IsRealValue( tmpdouble );
			ad_struct->__ptr[attr_index].value = (char*)soap_malloc(s,20);
			snprintf(ad_struct->__ptr[attr_index].value,20,"%f",tmpdouble);
			ad_struct->__ptr[attr_index].type = FLOAT_ATTR;
			break;
		case classad::Value::BOOLEAN_VALUE:
			val.IsBooleanValue( tmpbool );
			if ( tmpbool ) {
				ad_struct->__ptr[attr_index].value = "TRUE";
			} else {
				ad_struct->__ptr[attr_index].value = "FALSE";
			}
			ad_struct->__ptr[attr_index].type = BOOLEAN_ATTR;
			break;
		default:
			skip_attr = true;
		}
	} else {
		// assume everything else is some sort of expression
		tmpstr = NULL;
		int buflen = strlen( ExprTreeToString( tree ) );
		tmpstr = (char*)soap_malloc(s,buflen + 1); // +1 for termination
		ASSERT(tmpstr);
		tmpstr[0] = '\0';
		strcpy( tmpstr, ExprTreeToString( tree ) );
		if ( !(tmpstr[0]) ) {
			skip_attr = true;
		} else {
			ad_struct->__ptr[attr_index].value = tmpstr;
			ad_struct->__ptr[attr_index].type = EXPRESSION_ATTR;
		}
	}
    // skip this attr is requested to do so...
    if ( skip_attr ) continue;

    // serialize the attribute name, and finally increment our counter.
	if (isDeepCopy) {
		ad_struct->__ptr[attr_index].name =
			(char *)
			soap_malloc(s, strlen(name) + 1);
		strcpy(ad_struct->__ptr[attr_index].name, name);
	} else {
		ad_struct->__ptr[attr_index].name = (char *)name;
	}

    attr_index++;
    ad_struct->__size = attr_index;
  }

  return true;
}

static bool
convert_adlist_to_adStructArray(struct soap *s, List<ClassAd> *adList,
                                struct condor__ClassAdStructArray *ads)
{

  if ( !adList || !ads ) {
    return false;
  }

  ClassAd *curr_ad = NULL;
  ads->__size = adList->Number();
  ads->__ptr =
	  (struct condor__ClassAdStruct *)
	  soap_malloc(s, ads->__size * sizeof(struct condor__ClassAdStruct));
  adList->Rewind();
  int ad_index = 0;

  while ((curr_ad=adList->Next()))
    {
		if (convert_ad_to_adStruct(s,curr_ad,&(ads->__ptr[ad_index]), false)) {
        ad_index++;
      }
    }

  ads->__size = ad_index;
  return true;
}

static bool
convert_adStruct_to_ad(struct soap *s,
                       ClassAd *curr_ad,
                       struct condor__ClassAdStruct *ad_struct)
{
  MyString attribute;
  MyString name;
  MyString value;
  int i = 0;

  if (!ad_struct) {
    return false;
  }

  if (!curr_ad) {
    return false;
  }

  for (i = 0; i < ad_struct->__size; i++) {
    if (!ad_struct->__ptr[i].name)
      continue;
    else
      name = ad_struct->__ptr[i].name;

    if (!ad_struct->__ptr[i].value)
      value = "UNDEFINED";
    else
      value = ad_struct->__ptr[i].value;

		// XXX: This is ugly, but needed because of how MyType and TargetType
		// are treated specially in old classads.
	if (name == ATTR_MY_TYPE) {
		SetMyTypeName(*curr_ad, value.Value());
		continue;
	} else if (name == ATTR_TARGET_TYPE) {
		SetTargetTypeName(*curr_ad, value.Value());
		continue;
	}

    if (STRING_ATTR == ad_struct->__ptr[i].type)
      attribute = name + "=\"" + value + "\"";
    else
      attribute = name + "=" + value;

    curr_ad->Insert(attribute.Value());
  }

  return true;
}
