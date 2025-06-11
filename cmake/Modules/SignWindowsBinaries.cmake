function(sign_target_windows tgt)
    if(WIN32 AND GPT4ALL_SIGN_INSTALL)
        add_custom_command(TARGET ${tgt}
            POST_BUILD
            COMMAND AzureSignTool.exe sign
                -du "$ENV{SIGNING_URL}"
                -kvu $ENV{KVU_URL}
                -kvi "$Env{AZSignGUID}"
                -kvs "$Env{AZSignPWD}"
                -kvc "$Env{AZSignCertName}"
                -kvt "$Env{AZSignTID}"
                -tr http://timestamp.digicert.com
                -v
                $<TARGET_FILE:${tgt}>
        )
    endif()
endfunction()