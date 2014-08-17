ustring
=======

The following Unicode String support for PHP7 ...

```php
<?php
abstract class UString {
    public function __construct([string $string [, string $codepage]]);
    
    /** 
    * @return int
    **/
    public function length();
    
    /** 
    * @return void
    **/
    public function toUpper(void);
    
    /** 
    * @return void
    **/
    public function toLower(void);
    
    /** 
    * @return void
    **/
    public function reverse(void);
    
    /**
    * @return int new length
    **/
    public function trim(void);
    
    /** 
    * @return int new length
    **/
    public function truncate(int $length);
    
    /** 
    * @return int new length
    **/
    public function append(UString $string);
    
    /** 
    * @return int new length
    **/
    public function replace(UString $string, UString $replace);
    
    /**
    * @return bool
    **/
    public function startsWith(UString $needle);
    
    /**
    * @return bool
    **/
    public function endsWith(UString $needle);
    
    /**
    * @return int
    **/
    public function indexOf(UString $needle);
    
    /**
    * @return int
    **/
    public function lastIndexOf(UString $needle);
    
    /**
    * @return UString
    **/
    public function charAt(int $index);

    /**
    * @return void
    **/
    public function insert(int $position, UString $text [, int $start [, int $length]])
    
    /**
    * @return string
    **/
    public function getCodepage(void);
    
    /**
    * @return string
    **/
    public static function getDefaultCodepage();
    
    /**
    * @return void
    **/
    public static function setDefaultCodepage(string $codepage);
}
?>
```

*Note: UString is iterable and interchangable with zend strings for method parameters and can be cast for output/conversion to zend strings*

**This extension requires PHP7**
