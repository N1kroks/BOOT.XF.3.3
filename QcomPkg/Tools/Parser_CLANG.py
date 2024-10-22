import re
import MapParser

Debug_flag=False
################################################################################
# MapScannerTracker
################################################################################
class MapScannerTracker:
    """ Tracking info for scanning .map files

        This class is used to keep track of scanning data across calls of
        get_next_map_token.
    """
    def __init__(self, fh):
        """ Inits MapScannerTracker info """
        self.fh = fh
        self.split_line_section = False
        self.split_line_symbol = False
        self.split_line_common_symbol = False
        self.split_line_common_symbol_name = False
        self.curr_section = None
        self.curr_section_name = ''
        self.curr_section_addr = None
        self.curr_section_size = None
        self.prev_section = None
        self.prev_section_name = ''
        self.prev_section_addr = None
        self.prev_section_size = None
        self.prev_obj = None
        self.curr_symbol = None
        self.curr_obj = None
        self.curr_lib = None
        self.curr_segment = None

################################################################################
# get_next_map_token
################################################################################
def get_next_map_token(scanner):
    """ Parses next section/symbol from map file

        Scans through the .map file tracked by input MapScannerTracker looking
        for Symbols defined in .map file. Returns the first symbol encountered.

        Args:
            scanner: MapScannerTracker with current tracking info

        Returns:
            Symbol object representing next symbol found. None when EOF is 
            reached
    """
    for line in scanner.fh:
        # look for section header
        m = re.search('^([0-9_A-Z]+)'+\
                      '(\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(#)\s+([a-zA-Z]+:)' + \
                      '\s+(0x[0-9a-fA-F,]+)\s+([a-zA-Z:]+)\s+(0x[0-9a-fA-F]+))?\s*$',
                      line)
        if m:
            if m.group(2) != None:
                symbol = None
                #if section doesn't contain any obj and lib, just show this section as symbol
                if (scanner.prev_obj == None) and (scanner.prev_section != None):
                    symbol = MapParser.Symbol(scanner.prev_section_addr, 
                                              scanner.prev_section_size, 
                                             "NULL", 
                                              scanner.prev_section_name,
                                              "ZI",
                                              "NULL")
                                              
                section = MapParser.Section(m.group(1), 
                                            int(m.group(3),0), 
                                            int(m.group(4),0))
                scanner.curr_section_name = m.group(1)
                scanner.curr_section_addr = int(m.group(3),0)
                scanner.curr_section_size = int(m.group(4),0)
                scanner.prev_section = section
                scanner.prev_section_addr = scanner.curr_section_addr
                scanner.prev_section_size = scanner.curr_section_size
                scanner.curr_section = section
                scanner.split_line_common_symbol = False
                scanner.split_line_symbol = False
                scanner.curr_symbol = None
                scanner.curr_obj = None
                scanner.curr_lib = None
                if symbol:
                    return symbol
            else:
                scanner.curr_section_name = m.group(1)
                scanner.split_line_section = True
                if Debug_flag:
                    print "[Sector Head]"+section+"("+line+")"
            continue

        # handle split line header
        if scanner.split_line_section:
            m = re.search('^\s+(0x[0-9a-fA-F]*)\s+(0x[0-9a-fA-F]+)\s*$', line)
            scanner.split_line_section = False
            if m:
                section = MapParser.Section(scanner.curr_section_name,
                                            int(m.group(1),0),
                                            int(m.group(2),0))
                scanner.curr_section = section
            continue
			
		# Handle symbol	
        if scanner.curr_section != None:
            # look for COMMON symbol
            m = re.search('^([\*\.a-zA-Z0-9_]+)(\(\s?COMMON\s?\))\s*$',
                          line)
            if m:
              if scanner.curr_section != None:
                if m.group(2) == "(COMMON )":
                    scanner.split_line_common_symbol = True
                    scanner.curr_symbol = "COMMON"
                else:
                    scanner.split_line_common_symbol = False
                    scanner.curr_symbol = None
                continue
           
            # handle split line symbol
            if scanner.split_line_common_symbol:
                m = re.search('^\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+.*?' + \
                              '([^\\\\/]+\\.lib)\\((.*)\\)(\s+[#A-Z_,\|0-9]+)\s*$',
                              line)
                if m:
                    
                    scanner.curr_section_addr = int(m.group(1),0)
                    scanner.curr_section_size = int(m.group(2),0)
                    scanner.curr_obj = m.group(4)
                    scanner.curr_lib = m.group(3)
                    scanner.curr_segment = extract_segment(scanner.curr_symbol)
                    scanner.split_line_common_symbol_name = True
                    continue
                    #
                else:
                    m = re.search('^\s+(0x[0-9a-fA-F]*)\s+([0-9a-zA-Z_-]+)$',line)
                    if m:
                       if scanner.split_line_common_symbol_name:
                           symbol = MapParser.Symbol(scanner.curr_section_addr, 
                                              scanner.curr_section_size , 
                                              scanner.curr_obj, 
                                              scanner.curr_lib,
                                              scanner.curr_segment,
                                              m.group(2))
                           scanner.split_line_common_symbol_name = False                   
                       return symbol
                    else:
                        m = re.search('^(PADDING_ALIGNMENT)\s+(0x[0-9a-fA-f]+)\s+(0x[0-9a-fA-f]+)\s+(0x[0-9a-fA-f]+)$',line)
                        if m:  
                            symbol = MapParser.Symbol(int(m.group(2),0), 
                                                      int(m.group(3),0), 
                                                      scanner.curr_obj, 
                                                      scanner.curr_lib,
                                                      scanner.curr_segment,
                                                      #extract_segment(m.group(1)),
                                                      m.group(1))
                            scanner.curr_segment = extract_segment(m.group(1))
                            return symbol
                        else:
                            scanner.split_line_common_symbol = False
                            scanner.split_line_common_symbol_name = False 
                            #scanner.curr_symbol = None
                            #scanner.curr_obj = None
                            #scanner.curr_lib = None
            # look for other symbol
            m = re.search('^([\.a-zA-Z0-9_]+)(\s+(0x[0-9a-fA-F]+)\s+(0x+[0-9a-fA-F]+)\s+.*?([^\\/]+\.lib)\((.*)\))(\s+[#A-Z_,\|0-9]*)\s*$',line)
            if m and scanner.curr_section != None:
                scanner.curr_symbol = m.group(1)
                if m.group(2) != None:
                    symbol = MapParser.Symbol(int(m.group(3),0),
                                              int(m.group(4),0),
                                              m.group(6),
                                              m.group(5),
                                              extract_segment(m.group(1)),
                                              m.group(1))
                    scanner.curr_obj = m.group(6)
                    scanner.curr_lib = m.group(5)
                    scanner.curr_segment = extract_segment(m.group(1))
                    return symbol
                else:
                    scanner.split_line_symbol = True
                continue
           
            # handle split line symbol
            if scanner.split_line_symbol:
                m = re.search('^\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+.*?' + \
                              '([^\\\\/]+\\.lib)\\((.*)\\)\s*$',
                              line)
                scanner.split_line_symbol = False
                if m:
                    if Debug_flag:
                        print "catched symbol in the split line"
                    symbol = MapParser.Symbol(int(m.group(1),0),
                                              int(m.group(2),0),
                                              m.group(4),
                                              m.group(3),
                                              extract_segment(scanner.curr_symbol),
                                              scanner.curr_symbol)
                    scanner.curr_obj = m.group(4)
                    scanner.curr_lib = m.group(3)
                    scanner.curr_segment = extract_segment(scanner.curr_symbol)
                    return symbol
                continue
            #check PADDING_ALIGNMENT 
            m = re.search('^(PADDING_ALIGNMENT)\s+(0x[0-9a-fA-f]+)\s+(0x[0-9a-fA-f]+)\s+(0x[0-9a-fA-f]+)$',line)
            if m:
                symbol = MapParser.Symbol(int(m.group(2),0), 
                                          int(m.group(3),0), 
                                          scanner.curr_obj, 
                                          scanner.curr_lib,
                                          scanner.curr_segment,
                                          #extract_segment(m.group(1)),
                                          m.group(1))
                return symbol
           
            # end section on empty line
            m = re.search('^$', line)
            if m:
                scanner.prev_section_name = scanner.curr_section_name
                scanner.prev_obj = scanner.curr_obj
                scanner.split_line_section = False
                scanner.split_line_symbol = False
                scanner.curr_section = None
                scanner.curr_section_name = ''
                scanner.curr_symbol = None
                scanner.split_line_common_symbol = False
            else:
                continue
           
            # clear split line flags if no matches
            scanner.split_line_symbol = False
            scanner.split_line_common_symbol = False
    # indicate done scanning
    #return (None, None)
    return None

################################################################################
section_map = {
    'text' : 'CODE',
    'data' : 'RW',
    'bss' : 'ZI',
    'rodata' : 'RO',
    'COMMON' : 'ZI',
    'BOOT_UTIL_ASM' : 'CODE', 
    'CACHE_MMU_ZI' : 'ZI',
    'DDRSS_MEM_RDWR' : 'CODE',
    'ARM_MMU' : 'CODE',
    'SBL1_VECTOR_TABLE' : 'CODE',
    'SBL1_EXCEPTION_HANDLERS' : 'CODE',
    'XBL_SDI_ASM' : 'CODE',
    'SBL1_ENTRY' : 'CODE',
}

################################################################################
# extract_segment
################################################################################
def extract_segment(name):
    # find segment
    m = re.search('^\\.?([^\\.]+)', name)
    if m:
        name = m.group(1)
    
    # check that segment is defined
    if name in section_map:
        return section_map[name]

    else:
        return "OTHER"

################################################################################
# XMLScannerTracker
################################################################################
class XMLScannerTracker:
    """ Tracking info for scanning .xml files

        This class is used to keep track of scanning data across calls of
        get_next_xml_token.
    """
    def __init__(self, fh):
        """ Inits XMLScannerTracker info """
        self.fh = fh
        self.in_hw = False

################################################################################
# get_next_xml_token
################################################################################
def get_next_xml_token(scanner): 
    """ Parses next memory region from xml file

        Scans through the .xml file tracked by input XMLScannerTracker looking
        for hardware memory regions. Returns the first region found.

        Args:
            scanner: XMLScannerTracker with current tracking info

        Returns:
            None if scanner has finished file, otherwise Section containing
            info for next hardware memory regions encountered
    """
    for line in scanner.fh:
        # look for hardware section
        m = re.search('<Hardware>', line)
        if m:
            scanner.in_hw = True
            continue

        # look for hardware section end
        m = re.search('</Hardware>', line)
        if m:
            scanner.in_hw = False
            continue

        # look for bank in hw section
        if scanner.in_hw:
            m = re.search('<Bank name=\"([^\"]+)\" addr=\"([^\"]+)\" ' + \
                          'size=\"([^\"]+)\"/>', 
                          line)
            if m:
                size = m.group(3)
                size = size.replace("K", "* 1024")
                size = size.replace("M", "* 1024 * 1024")
                size = size.replace("G", "* 1024 * 1024 * 1024")
                section = MapParser.Section(m.group(1), 
                                            int(m.group(2),0), 
                                            eval(size))
                return section

    return None

################################################################################
# LDScannerTracker
################################################################################
class LDScannerTracker:
    """ Tracking info for scanning .ld files

        This class is used to keep track of scanning data across calls of
        get_next_ld_token.
    """
    def __init__(self, fh):
        """ Init LDScannerTracker info """
        self.fh = fh
        self.in_sections = False
        self.brace_depth = 0
        self.in_section = False
        self.section_types = []

################################################################################
# get_next_ld_token
################################################################################
def get_next_ld_token(scanner):

    #for line in scanner.fh:
        # look for sections
    #    m = re.search('SECTIONS', line)
    #    if m:
    #        scanner.in_sections = True

        # look for braces
    #    m = re.search('{', line)
    #    if m:
    #        scanner.brace_depth += 1

     #   m = res.search('}', line)
     #   if m:
   #         scanner.brace_depth -= 1

        # check to see if in section
    #    if (scanner.in_sections and scanner.brace_depth == 2):
     #       scanner.in_section = True

        # look for entries in section
      #  if scanner.in_section:
       #     # look for named section
        #    m = re.search('\(([A-Z_]+)\)', line)
         #   if m:
                
        

    return None
