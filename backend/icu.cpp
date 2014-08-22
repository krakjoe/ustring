/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2014 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: krakjoe@php.net                                              |
  +----------------------------------------------------------------------+
*/
#ifndef PHP_USTRING_BACKEND_ICU
#define PHP_USTRING_BACKEND_ICU

static inline void _php_ustring_construct(zval *that, const char *value, zend_size_t vlen, const char *codepage, zend_size_t clen TSRMLS_DC) {
    php_ustring_t* ustring;
    
    if (Z_TYPE_P(that) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(that), ce_UString TSRMLS_CC)) {
        return;
    }
    
    ustring = php_ustring_fetch(that);
    
    if (!codepage) {
        ustring->codepage = STR_COPY(UG(codepage));
    } else ustring->codepage = STR_INIT(codepage, clen, 0);
    
    ustring->val = new UnicodeString(value, vlen, ustring->codepage->val);
}

static inline zend_size_t _php_ustring_length(zval *that TSRMLS_DC) {
    return (php_ustring_fetch(that))->val->length();
}

static inline bool _php_ustring_startsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    uneedle = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    uneedle = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return false;
	}
	
    return ustring->val->startsWith(uneedle);
}

static inline bool _php_ustring_endsWith(zval *that, zval *needle TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString uneedle;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    uneedle = UnicodeString(Z_STRVAL_P(needle), (int32_t)Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    uneedle = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return false;
	}
	
    return ustring->val->endsWith(uneedle);
}

static inline zend_int_t _php_ustring_indexOf(zval *that, zval *needle, zend_int_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else h = *ustring->val;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    if (offset)
		        n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		    else n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    n = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return -1;
	}
	
    return h.indexOf(n);
}

static inline zend_int_t _php_ustring_lastIndexOf(zval *that, zval *needle, zend_int_t offset TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString n, h;
    
    if (offset) {
        h = UnicodeString(*ustring->val, offset);
    } else h = *ustring->val;
    
    switch (Z_TYPE_P(needle)) {
		case IS_STRING:
		    if (offset)
		        n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		    else n = UnicodeString(Z_STRVAL_P(needle), (int32_t) Z_STRSIZE_P(needle), ustring->codepage->val);
		break;

		case IS_OBJECT:
		    n = *(php_ustring_fetch(needle))->val;
		break;
		
		default:
		    return -1;
	}
	
    return h.lastIndexOf(n);
}

static inline zval* _php_ustring_toLower(zval *that, zval *lowered TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(lowered, ce_UString);
    
    ostring = php_ustring_fetch(lowered);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->toLower();
    ostring->codepage = STR_COPY(ustring->codepage);
    
    return lowered;
}

static inline zval* _php_ustring_toUpper(zval *that, zval *uppered TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(uppered, ce_UString);
    
    ostring = php_ustring_fetch(uppered);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->toUpper();
    ostring->codepage = STR_COPY(ustring->codepage);
    
    return uppered;
}

static inline zval* _php_ustring_reverse(zval *that, zval *reversed TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(reversed, ce_UString);
    
    ostring = php_ustring_fetch(reversed);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->reverse();
    ostring->codepage = STR_COPY(ustring->codepage);

    return reversed;
}

static inline zval* _php_ustring_trim(zval *that, zval *trimmed TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
    
    object_init_ex(trimmed, ce_UString);
    
    ostring = php_ustring_fetch(trimmed);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->val->trim();
    ostring->codepage = STR_COPY(ustring->codepage);
    
    return trimmed;
}

static inline zval* _php_ustring_replace(zval *that, zval *search, zval *replace, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;
                  
     UnicodeString s, r;
    
	switch (Z_TYPE_P(search)) {
		case IS_STRING: {
			s = UnicodeString(Z_STRVAL_P(search), (int32_t) Z_STRSIZE_P(search), ustring->codepage->val);

			switch(Z_TYPE_P(replace)) {
				case IS_STRING:
					r = UnicodeString(Z_STRVAL_P(replace), (int32_t) Z_STRSIZE_P(replace), ustring->codepage->val);
				break;

				case IS_OBJECT: {
					r = *(php_ustring_fetch(replace))->val;
				} break;

				default:
					return NULL;
			}
		}
		break;

		case IS_OBJECT: {
			s = *(php_ustring_fetch(search))->val;

			switch(Z_TYPE_P(replace)) {
				case IS_OBJECT:
					r = *(php_ustring_fetch(replace))->val;
				break;

				case IS_STRING:
					r = UnicodeString(Z_STRVAL_P(replace), (int32_t) Z_STRSIZE_P(replace), ustring->codepage->val);
				break;

				default:
					return NULL;
			}
		}
		break;

		default:
			return NULL;
	}
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_fetch(replaced);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->codepage = STR_COPY(ustring->codepage);
	ostring->val->findAndReplace(s, r);
	
	return replaced;
}

static inline zval* _php_ustring_replaceSlice(zval *that, zval *slice, zend_int_t offset, zend_int_t length, zval *replaced TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring;

    UnicodeString s;
    
    switch (Z_TYPE_P(slice)) {
		case IS_STRING:
			s = UnicodeString(Z_STRVAL_P(slice), (int32_t) Z_STRSIZE_P(slice), ustring->codepage->val);
			break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(slice), ce_UString TSRMLS_CC)) {
				return NULL;
			}

			s = *(php_ustring_fetch(slice))->val;
			break;

		default:
			return NULL;
	}

	if (offset == -1)
		offset = 0;
	if (length == -1)
		length = s.length() - offset;
    
    object_init_ex(replaced, ce_UString);
    
    ostring = php_ustring_fetch(replaced);
    ostring->val = new UnicodeString(*ustring->val);
    ostring->codepage = STR_COPY(ustring->codepage);
    ostring->val->replace(offset, length, s, 0, s.length());
    
    return replaced;
}

static inline zval* _php_ustring_charAt(zval *that, zend_int_t offset, zval *found TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
                  *ostring = NULL;

    UChar c = ustring->val->charAt(offset);

	if (c) {
		object_init_ex(found, ce_UString);

		ostring = php_ustring_fetch(found);
		ostring->val = new UnicodeString(&c, 1);
		ostring->codepage = STR_COPY(ustring->codepage);
	}
	
	return found;
}

static inline zval* _php_ustring_substring(zval *that, zend_int_t offset, zend_int_t length, zval *sub TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that),
				  *ostring;
    
	if (offset < 0) {
	    return NULL;
	}
	
	if (length == -1)
	    length = ustring->val->length() - offset;
	    
	object_init_ex(sub, ce_UString);
	
	ostring = php_ustring_fetch(sub);
	ostring->codepage = STR_COPY(ustring->codepage);
	ostring->val = new UnicodeString(*ustring->val, offset, length);
	
	return sub;
}

static inline bool _php_ustring_contains(zval *that, zval *text TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);

    UnicodeString t;
    
    switch (Z_TYPE_P(text)) {
        case IS_STRING:
            t = UnicodeString(Z_STRVAL_P(text), (int32_t) Z_STRSIZE_P(text), ustring->codepage->val);
        break;
        
        case IS_OBJECT:
            if (!instanceof_function(Z_OBJCE_P(text), ce_UString TSRMLS_CC)) {
                return false;
            }
            
            t = *(php_ustring_fetch(text))->val;
        break;
    }
    
    return (ustring->val->indexOf(t) >= 0);
}

static inline zval* _php_ustring_chunk(zval *that, zend_int_t length, zval *chunks TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(that);
    zend_int_t position = 0;
    
    array_init(chunks);
    
    if (!length) 
        length = 1;
    
    while (position < ustring->val->length()) {
        zval chunked;
        php_ustring_t *uchunk;
        
        object_init_ex(&chunked, ce_UString);
        
        uchunk = php_ustring_fetch(&chunked);
        uchunk->codepage = STR_COPY(ustring->codepage);
        uchunk->val = new UnicodeString(*ustring->val, position, length);
        
        add_next_index_zval(chunks, &chunked);
        
        position += length;
    }
    
    return chunks;
}

static inline zval* _php_ustring_repeat(zval *that, zend_int_t count, zval *repeated TSRMLS_DC) {
    php_ustring_t *ustring = php_ustring_fetch(getThis()), 
                  *urepeat;
    zend_int_t current = 0;
    
    object_init_ex(repeated, ce_UString);
    
    urepeat = php_ustring_fetch(repeated);
    urepeat->codepage = STR_COPY(ustring->codepage);
    urepeat->val = new UnicodeString(*ustring->val);
    
    while (++current < count) {
        urepeat->val->append((*ustring->val));
    }
    
    return repeated;
}

static inline int _php_ustring_compare(zval *op1, zval *op2 TSRMLS_DC) {
	UnicodeString us1, us2;

	switch (Z_TYPE_P(op1)) {
		case IS_STRING:
			us1 = UnicodeString(Z_STRVAL_P(op1), (int32_t) Z_STRSIZE_P(op1), UG(codepage)->val);
		break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(op1), ce_UString TSRMLS_CC)) {
				return 0;
			}

			us1 = *(php_ustring_fetch(op1))->val;
		break;

		default:
			return 0;
	}

	switch (Z_TYPE_P(op2)) {
		case IS_STRING:
			us2 = UnicodeString(Z_STRVAL_P(op2), (int32_t) Z_STRSIZE_P(op2), UG(codepage)->val);
		break;

		case IS_OBJECT:
			if (!instanceof_function(Z_OBJCE_P(op2), ce_UString TSRMLS_CC)) {
				return 0;
			}

			us2 = *(php_ustring_fetch(op2))->val;
		break;

		default:
			return 0;
	}

	return us1.compare(us2);
}

static inline void* _php_ustring_value(zval *that TSRMLS_DC) {
    return php_ustring_fetch(that)->val;
}

static inline zend_string* _php_ustring_getCodepage(zval *that TSRMLS_DC) {
    return (php_ustring_fetch(that))->codepage;
}

php_ustring_handlers_t php_ustring_defaults = {
    _php_ustring_construct,
    _php_ustring_length,
    _php_ustring_startsWith,
    _php_ustring_endsWith,
    _php_ustring_indexOf,
    _php_ustring_lastIndexOf,
    _php_ustring_toLower,
    _php_ustring_toUpper,
    _php_ustring_reverse,
    _php_ustring_trim,
    _php_ustring_replace,
    _php_ustring_replaceSlice,
    _php_ustring_charAt,
    _php_ustring_substring,
    _php_ustring_contains,
    _php_ustring_chunk,
    _php_ustring_repeat,
    _php_ustring_compare,
    _php_ustring_value,
    _php_ustring_getCodepage,
};

#endif
