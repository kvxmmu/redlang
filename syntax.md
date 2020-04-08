# RedLang syntax

Example:

    import top_level_import.lower_level_import; // comment
    
    /* comment */
    
    func main(string[] args) -> int32 {
        var auto_detect_type = 100;
        string string_type = "string literal";
        char single_character = 'h';
        
        print("Args count - %d" % args.length);
        print("File path - %s" % args[0]);
        
        print(i"Interpolation example ${single_character}");
        return 0;
    }