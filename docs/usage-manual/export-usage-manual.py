# pip install selenium
# I had to also do pip install --upgrade requests
# Download geckodriver from here https://github.com/mozilla/geckodriver/releases and put it in Downloads
# sudo chmod +x geckodriver
# export PATH=$PATH:/home/marc/Downloads (or wherever you put it)

from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import time
from html.parser import HTMLParser
import os

# Settings
pages_to_save = ['Handling Flowgraphs',
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
# dir that contains geckodriver
driver = webdriver.Firefox('/home/marc/Downloads/geckodriver')
print("STARTING")
for page_name in pages_to_save:
    print("Processing", page_name)
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
    h = HTMLParser()
    # makes it so stuff like &gt shows up as a greater than sign
    cropped_html_text = h.unescape(cropped_html)
    text_file = open("(exported from wiki) " + page_name + ".txt", "w")
    text_file.write(cropped_html_text)
    text_file.close()

driver.close()
print("DONE")
