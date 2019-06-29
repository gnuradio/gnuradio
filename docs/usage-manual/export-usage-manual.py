from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time
import HTMLParser
import os

# Settings
pages_to_save = ['GNURadioCompanion',
                 'Handling Flowgraphs',
                 'Types of Blocks',
                 'Metadata Information',
                 'Stream Tags',
                 'Block Thread Affinity and Priority',
                 'Configuration Files',
                 'VOLK Guide',
                 'Polymorphic Types (PMTs)',
                 'Message Passing',
                 'QT GUI',
                 'Logging',
                 'Performance Counters',
                 'Tagged Stream Blocks',
                 'Polyphase Filterbanks']

# set up web driver
driver = webdriver.Firefox()
for page_name in pages_to_save:
    driver.get("https://wiki.gnuradio.org/index.php/Special:Export")

    # fill in text box
    text_area = driver.find_element_by_xpath("//*[@name='pages']")
    text_area.send_keys(page_name)

    # uncheck "save as file" box
    check_box = driver.find_element_by_xpath("//*[@name='wpDownload']")
    check_box.click()

    # hit Export
    submit_button = driver.find_element_by_xpath("//*[@value='Export']")
    submit_button.click() 

    # get HTML of new page
    raw_html = driver.page_source
    start_index = raw_html.find('<page>')
    cropped_html = raw_html[start_index:]

    # save text to file
    h = HTMLParser.HTMLParser()
    cropped_html_text = h.unescape(cropped_html) # makes it so stuff like &gt shows up as a greater than sign
    text_file = open("(exported from wiki) " + page_name + ".txt", "w")
    text_file.write(cropped_html_text)
    text_file.close()

driver.close()
