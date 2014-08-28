[![Build Status](https://travis-ci.org/krakjoe/ustring.svg?branch=master)](https://travis-ci.org/krakjoe/ustring)

ustring
=======

Provides a simple interface to work with Unicode strings in upcoming PHP7.


```php
<?php
abstract class UString implements Iterable {
    /**
    * Shall create a UString with the given value and codepage.
    * If no codepage is provided, the default codepage will be used.
    * The default codepage is set to UTF-8 by default
    * @see UString::setDefaultCodepage
    * @see UString::getDefaultCodepage
    **/
    public function __construct([string $string [, string $codepage] ]);
    
    /** 
    * Shall return the number of unicode code points
    * @return int
    **/
    public function length();
    
    /** 
    * Shall return a new UString whose characters are converted to upper case
    * @return UString
    **/
    public function toUpper(void);
    
    /** 
    * Shall return a new UString whose characters are converted to lower case
    * @return UString
    **/
    public function toLower(void);
    
    /** 
    * Shall return a new UString whose characters have been reversed
    * @return UString
    **/
    public function reverse(void);
    
    /**
    * Shall return a new UString with the leading and trailing space removed
    * @return UString
    **/
    public function trim(void);
    
    /**
    * Shall return a new UString with every occurence of $search having been replaced with $replace
    * @return UString
    **/
    public function replace(UString $search, UString $replace);
    
    /**
    * Shall return a new UString with the substring starting at $start and having length $length replaced by $text.
    * @return UString
    **/
    public function replaceSlice(UString $text [, int $start [, int $length]])
    
    /**
    * Shall return positively if this contains $text
    * @return bool
    **/
    public function contains(UString $text);
    
    /**
    * Shall return positively if this starts with $needle
    * @return bool
    **/
    public function startsWith(UString $needle);
    
    /**
    * Shall return positively if this ends with $needle
    * @return bool
    **/
    public function endsWith(UString $needle);
    
    /**
    * Shall return the index of the first occurence of $needle, with the search starting at $offset
    * @return mixed
    **/
    public function indexOf(UString $needle [, int $offset]);
    
    /**
    * Shall return the index of the last occurence of $needle, with the search starting at $offset
    * @return mixed
    **/
    public function lastIndexOf(UString $needle [, int $offset]);
    
    /**
    * Shall return an array of UString containing chunks of this, each with a maximum length of $length
    * @return UString[]
    **/
    public function chunk(int $length);
    
    /**
    * Shall return a new UString with the contents of this repeated $count times
    * @return UString
    **/
    public function repeat(int $count);
    
    /**
    * @return UString
    **/
    public function pad(int $length [, UString $pad = ' ' [, int $mode = STR_PAD_RIGHT ] ]);
    
    /**
    * @return UString[]
    **/
    public function split(UString $delimiter [, int $limit = NULL]);
    
    /**
    * @return UString
    **/
    public function charAt(int $index);
    
    /**
    * Shall return a new UString containing a substring of this, starting at $start with a maximum length of $length
    * If $length is not provided shall use UnicodeString::length() - $start to determine the length of the substring to return
    * @return UString
    **/
    public function substring(int $start [, int $length]);
    
    /**
    * Shall return the codepage of this
    * @return string
    **/
    public function getCodepage(void);
    
    /**
    * Shall return the default codepage
    * @return string
    **/
    public static function getDefaultCodepage();
    
    /**
    * Shall set the default codepage
    * @return void
    **/
    public static function setDefaultCodepage(string $codepage);
}

/**
* A convenience shortcut for UString constructor is registered by the extension
* @return UString
**/
function u(string $value);
?>
```

This API is derived, in part, from work done by Nikita Poppov, which can be found here: https://github.com/nikic/scalar_objects/blob/master/doc/string_api.md

*Note: UString is interchangable with zend strings for method parameters and can be cast for output/conversion to zend strings*

**This extension requires PHP7 and ICU (with headers)**
