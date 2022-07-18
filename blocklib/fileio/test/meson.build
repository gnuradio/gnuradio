###################################################
#    QA
###################################################

if get_option('enable_testing')
    test('qa_file_source', py3, args : files('qa_file_source.py'), env: TEST_ENV)
    test('qa_file_sink', py3, args : files('qa_file_sink.py'), env: TEST_ENV)

endif
