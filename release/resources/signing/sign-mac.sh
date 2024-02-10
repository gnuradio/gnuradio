echo $MAC_SIGNING_CERT | base64 --decode > cert.p12
security create-keychain -p "$MAC_KEYCHAIN_KEY" sign.keychain
security default-keychain -s sign.keychain
security unlock-keychain -p "$MAC_KEYCHAIN_KEY" sign.keychain
security import cert.p12 -k sign.keychain -P "$MAC_SIGNING_CERT_PWD" -T /usr/bin/codesign
security set-key-partition-list -S apple-tool:,apple:,codesign: -s -k "$MAC_KEYCHAIN_KEY" sign.keychain
rm cert.p12
codesign --options runtime --timestamp -s "$MAC_SIGNING_CERT_NAME" $1
xcrun notarytool submit $1 --apple-id "$MAC_NOTARIZATION_ID" --team-id "$MAC_NOTARIZATION_TID" --password "$MAC_NOTARIZATION_KEY" --wait  | tee notarize_log.txt
xcrun stapler staple $1
