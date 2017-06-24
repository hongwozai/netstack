((nil (ffip-project-root . "/home/lm/workspace/netstack/")
      (ffip-patterns     . ("*.[ch]" "*.cpp" "*.hpp" "*.S" "*.md" "*.org"))
      (ffip-prune-patterns . ("*/.*/*" "*/.git/*" "*/.svn/*"))
      (ffip-find-options . "-not -size +64k -not -iwholename '*/dist/*'"))

 (c++-mode . ((flycheck-clang-include-path . ("../inc" "../utils"))
              (company-c-headers-path-system . ("/usr/include/"
                                                "/usr/local/include/"
                                                "../inc" "../utils"))))
 )
